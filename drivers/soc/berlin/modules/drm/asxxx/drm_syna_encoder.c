// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 *
 * Author: Prem Anand N <pranan@synaptics.com>
 *
 */

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include "drm_syna_drv.h"
#include <drm/drm_panel.h>

#include "dsih_displays.h"
#include "dsih_core.h"
#include "dsih_api.h"
#include "includes.h"
#include "vpp_api.h"

/* To accomodate lower pixel clocks, set pixel clock with mulitplier
 * Add Corresponding divider in the Driver
 */
#define PIXEL_CLOCK_MULTIPLIER				4
#define PIXEL_CLOCK_RATE(PIX_CLK)			(PIX_CLK * PIXEL_CLOCK_MULTIPLIER * 1000)

typedef struct dsi_desc_t {
	int    outformat;
	int    bpp;
	int    byteclock;
	int    lpcmd;
	int    non_continuous_clock;
	int    lanes;
	int    eotp_tx;
	int    datapolarity;
	int    eotp_rx;
	int    h_polarity;
	int    v_polarity;
	int    data_polarity;
	int    cmdsize;
	int    htotal;
	int    video_mode;
	int    receive_ack;
	int    is_18_loose;
	int    color_coding;
	int    chunks;
	int    null_pkt;
} SYNA_DSI_DESC;

static SYNA_DSI_DESC    synaDsiInfo;
static struct mipi_dsi_dev *dsi_dev = NULL;

static int lcdc_rgb_swap;

static void syna_dsi_host_config(void)
{
	dsi_platform_init(dsi_dev, 0, VIDEO_MODE, synaDsiInfo.lanes);
}

static void syna_update_dsi_info(void)
{
	dsi_dev->dpi_video.virtual_channel = 0;
	dsi_dev->dpi_video.display_type = 0;

	dsi_dev->dpi_video.no_of_lanes = synaDsiInfo.lanes;
	dsi_dev->dpi_video.video_mode = synaDsiInfo.video_mode;
	dsi_dev->dpi_video.receive_ack_packets = synaDsiInfo.receive_ack;
	dsi_dev->dpi_video.is_18_loosely = synaDsiInfo.is_18_loose;
	dsi_dev->dpi_video.data_en_polarity = synaDsiInfo.data_polarity;

	dsi_dev->dpi_video.h_polarity = synaDsiInfo.h_polarity;
	dsi_dev->dpi_video.v_polarity = synaDsiInfo.v_polarity;
	dsi_dev->dpi_video.color_coding = synaDsiInfo.color_coding;

	dsi_dev->dpi_video.eotp_rx_en = synaDsiInfo.eotp_rx;
	dsi_dev->dpi_video.eotp_tx_en = synaDsiInfo.eotp_tx;

	dsi_dev->dpi_video.non_continuous_clock = synaDsiInfo.non_continuous_clock;
	dsi_dev->dpi_video.no_of_chunks = synaDsiInfo.chunks;
	dsi_dev->dpi_video.null_packet_size = synaDsiInfo.null_pkt;
	dsi_dev->dpi_video.dpi_lp_cmd_en = synaDsiInfo.lpcmd;
	dsi_dev->dpi_video.byte_clock = synaDsiInfo.byteclock; /* KHz  */
	dsi_dev->dpi_video.h_total_pixels = synaDsiInfo.htotal;
}

static int syna_encoder_parse_lcdc_dt(void)
{
	struct device_node *lcdc_node;

	lcdc_node = of_find_compatible_node(NULL, NULL, "syna,drm-lcdc");

	if (!lcdc_node) {
		DRM_ERROR("LCDC node not found \n");
		return -ENODEV;
	}

	of_property_read_u32(lcdc_node, "rgbswap",  &lcdc_rgb_swap);

	return 0;
}

static int syna_encoder_parse_dsi_dt(void)
{
	struct device_node *dsi_node;

	dsi_node = of_find_compatible_node(NULL, NULL, "syna,drm-dsi");

	if (!dsi_node) {
		DRM_ERROR("DSI node not found \n");
		return -ENODEV;
	}

	of_property_read_u32(dsi_node, "htotal",  &synaDsiInfo.htotal);
	of_property_read_u32(dsi_node, "byte_clock",  &synaDsiInfo.byteclock);
	of_property_read_u32(dsi_node, "bits_per_pixel", &synaDsiInfo.bpp);
	of_property_read_u32(dsi_node, "busformat", &synaDsiInfo.outformat);
	of_property_read_u32(dsi_node, "lanes", &synaDsiInfo.lanes);
	of_property_read_u32(dsi_node, "video_mode", &synaDsiInfo.video_mode);
	of_property_read_u32(dsi_node, "receive_ack", &synaDsiInfo.receive_ack);
	of_property_read_u32(dsi_node, "is_18_loose", &synaDsiInfo.is_18_loose);
	of_property_read_u32(dsi_node, "h_pol", &synaDsiInfo.h_polarity);
	of_property_read_u32(dsi_node, "v_pol", &synaDsiInfo.v_polarity);
	of_property_read_u32(dsi_node, "data_pol", &synaDsiInfo.data_polarity);
	of_property_read_u32(dsi_node, "eotp_tx", &synaDsiInfo.eotp_tx);
	of_property_read_u32(dsi_node, "eotp_rx", &synaDsiInfo.eotp_rx);
	of_property_read_u32(dsi_node, "non_continuous_clock", &synaDsiInfo.non_continuous_clock);
	of_property_read_u32(dsi_node, "lp_cmd", &synaDsiInfo.lpcmd);
	of_property_read_u32(dsi_node, "color_coding", &synaDsiInfo.color_coding);
	of_property_read_u32(dsi_node, "chunks", &synaDsiInfo.chunks);
	of_property_read_u32(dsi_node, "null_pkt", &synaDsiInfo.null_pkt);

	return 0;
}

static void
syna_encoder_helper_mode_set(struct drm_encoder *encoder,
				  struct drm_display_mode *mode,
				  struct drm_display_mode *adjusted_mode)
{
	struct syna_drm_private *dev_priv = encoder->dev->dev_private;
	SYNA_LCDC_CONFIG lcdcConfig = {0};
	struct drm_display_info *disp_info = &dev_priv->connector[encoder->index]->display_info;

	if (disp_info->num_bus_formats) {
		lcdcConfig.mode = *disp_info->bus_formats;
		lcdcConfig.bits_per_pixel = disp_info->bpc;
	}

	/*htotal = hsync_len + left_margin + xres + right_margin*/
	lcdcConfig.hsync_len = mode->hsync_end - mode->hsync_start;
	lcdcConfig.right_margin = mode->hsync_start - mode->hdisplay; //FP
	lcdcConfig.xres = mode->hdisplay;
	lcdcConfig.left_margin = mode->htotal - mode->hsync_end;  //BP

	/*vtotal = vsync_len + upper_margin + yres + lower_margin*/
	lcdcConfig.vsync_len = mode->vsync_end - mode->vsync_start;
	lcdcConfig.lower_margin = mode->vsync_start - mode->vdisplay; //FP
	lcdcConfig.yres = mode->vdisplay;
	lcdcConfig.upper_margin = mode->vtotal - mode->vsync_end; //BP
	lcdcConfig.pixclock = mode->clock ;

	if (encoder->index) {
		/* Update the timing info */
		dsi_dev->dpi_video.h_sync_pixels = lcdcConfig.hsync_len;
		dsi_dev->dpi_video.h_back_porch_pixels = lcdcConfig.left_margin;
		dsi_dev->dpi_video.v_sync_lines = lcdcConfig.vsync_len;
		dsi_dev->dpi_video.v_back_porch_lines = lcdcConfig.upper_margin;
		dsi_dev->dpi_video.h_active_pixels = lcdcConfig.xres;
		dsi_dev->dpi_video.v_active_lines = lcdcConfig.yres;
		dsi_dev->dpi_video.pixel_clock = lcdcConfig.pixclock;
		dsi_dev->dpi_video.v_total_lines = lcdcConfig.yres +
											lcdcConfig.lower_margin +
											lcdcConfig.vsync_len +
											lcdcConfig.upper_margin;

		/* Configure DSI host */
		syna_dsi_host_config( );
		VPP_Clock_Set_Rate_Ext(PIXEL_CLOCK_RATE(lcdcConfig.pixclock));
	} else {
		lcdcConfig.rgb_swap = lcdc_rgb_swap;
		VPP_Clock_Set_Rate(PIXEL_CLOCK_RATE(lcdcConfig.pixclock));
	}

	if(dev_priv->connector[encoder->index] && dev_priv->panel[encoder->index])
		drm_panel_prepare(dev_priv->panel[encoder->index]);

	syna_vpp_load_config(encoder->index, &lcdcConfig);
}

static void syna_encoder_destroy(struct drm_encoder *encoder)
{
	if (encoder == NULL) {
		DRM_ERROR("%s: encoder is NULL!!\n", __func__);
		return;
	}
	DRM_DEBUG_DRIVER("[ENCODER:%d:%s]\n", encoder->base.id, encoder->name);

	drm_encoder_cleanup(encoder);

	kfree(dsi_dev);
	kfree(encoder);
}

static const struct drm_encoder_helper_funcs syna_encoder_helper_funcs = {
	.mode_set = syna_encoder_helper_mode_set,
};

static const struct drm_encoder_funcs syna_encoder_funcs = {
	.destroy = syna_encoder_destroy,
};

struct drm_encoder *syna_encoder_create(struct drm_device *dev,
		ENUM_VOUT_CONNECTOR vout_id, ENUM_CPCB_ID cpcb_id)
{
	struct drm_encoder *encoder;
	struct platform_device *pdev = to_platform_device(dev->dev);
	int err;
	int encoder_type = (cpcb_id == 0) ? \
						DRM_MODE_ENCODER_DPI :\
						DRM_MODE_ENCODER_DSI;

	if (encoder_type == DRM_MODE_ENCODER_DSI) {
		dsi_dev = devm_kzalloc(&pdev->dev, sizeof(struct mipi_dsi_dev), GFP_KERNEL);

		if (!dsi_dev) {
			DRM_ERROR("Allocate memory failed %ld\n", sizeof(struct mipi_dsi_dev));
			return ERR_PTR(-ENOMEM);
		}

		err = syna_encoder_parse_dsi_dt( );
		if (err) {
			DRM_ERROR("DSI Encoder info not available\n");
			return ERR_PTR(err);
		}
		mipi_dsi_init(dsi_dev);
		syna_update_dsi_info();

		dsi_register_device(dsi_dev);
	} else {
		syna_encoder_parse_lcdc_dt();
	}

	encoder = kzalloc(sizeof(*encoder), GFP_KERNEL);
	if (!encoder)
		return ERR_PTR(-ENOMEM);

	err = drm_encoder_init(dev, encoder, &syna_encoder_funcs,
					encoder_type, NULL);
	if (err) {
		DRM_ERROR("Failed to initialise encoder - %d", vout_id);
		kfree(encoder);
		return ERR_PTR(err);
	}

	drm_encoder_helper_add(encoder, &syna_encoder_helper_funcs);

	/*
	 * This is a bit field that's used to determine which
	 * CRTCs can drive this encoder.
	 */
	encoder->possible_crtcs = 0x1 << cpcb_id;

	DRM_DEBUG_DRIVER("[ENCODER:%d:%s]\n", encoder->base.id, encoder->name);

	return encoder;
}

void drm_syna_encoder_suspend(struct syna_drm_private *dev_priv,
							bool suspend)
{
	int panelId;

	for (panelId = 0; panelId < MAX_PANELS; panelId++) {
		if (dev_priv->connector[panelId] && dev_priv->panel[panelId]) {
			if (suspend)
				drm_panel_unprepare(dev_priv->panel[panelId]);
			else
				drm_panel_prepare(dev_priv->panel[panelId]);
		}
	}
}
