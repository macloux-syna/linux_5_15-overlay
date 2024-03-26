// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 *
 * Author: Prem Anand <prem.anand@synaptics.com>
 *
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>

#include <drm/drm_crtc.h>
#include <drm/drm_device.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>

#include "panel/panel.h"
#include "dsih_core.h"
#include "dsih_api.h"
#include "includes.h"
#include "dsih_displays.h"

typedef struct panel_timing_info_t {
	unsigned int hact;
	unsigned int hfp;
	unsigned int hsa;
	unsigned int hbp;
	unsigned int vact;
	unsigned int vfp;
	unsigned int vsa;
	unsigned int vbp;
	unsigned int pixelclock;

} PANEL_TIMING_INFO;

typedef struct panel_desc_t {
	PANEL_TIMING_INFO   panel_timing;
	int                 outformat;
	int                 bpp;
	int                 byteclock;
	int                 lpcmd;
	int                 non_continuous_clock;
	int                 lanes;
	int                 eotp_tx;
	int                 datapolarity;
	int                 eotp_rx;
	int                 h_polarity;
	int                 v_polarity;
	int                 data_polarity;
	int                 cmdsize;
	int                 htotal;
	int                 video_mode;
	int                 receive_ack;
	int                 is_18_loose;
	int                 color_coding;
	int                 chunks;
	int                 null_pkt;
	unsigned char		*cmd;
	struct gpio_desc 	*mipirst;
	struct gpio_desc 	*mipibl;
} SYNA_PANEL_DESC;

static struct display_timing synaPanelTimings;
static SYNA_PANEL_DESC  	synaPanelInfo;
static 	struct drm_panel *dsi_panel;

static const struct of_device_id platform_of_match[] = {
	{
		.compatible = "syna,drm-dsi",
	},
};

static int syna_panel_dsi_get_timings (struct drm_panel *panel, unsigned int num_timings,
			   struct display_timing *dptimings)
{
	memcpy(dptimings, &synaPanelTimings, sizeof(struct display_timing));

	return 0;
}

static int syna_panel_dsi_prepare(struct drm_panel *panel)
{
	/* Release MIPI from Reset */
	gpiod_set_value_cansleep(synaPanelInfo.mipirst, 0);
	dsi_panel_send_cmd(synaPanelInfo.cmdsize, synaPanelInfo.cmd);

	return 0;
}

static int syna_panel_dsi_unprepare(struct drm_panel *panel)
{
	/* Reset the MIPI */
	gpiod_set_value_cansleep(synaPanelInfo.mipirst, 1);

	return 0;
}

static int syna_panel_dsi_get_modes(struct drm_panel *panel, struct drm_connector *connector) {

	struct drm_display_mode *dmode;

	dmode = drm_mode_create(connector->dev);
	if (dmode) {
		dmode->hdisplay = synaPanelTimings.hactive.typ;
		dmode->hsync_start = dmode->hdisplay + synaPanelTimings.hfront_porch.typ;
		dmode->hsync_end =  dmode->hsync_start + synaPanelTimings.hsync_len.typ;
		dmode->htotal = dmode->hsync_end + synaPanelTimings.hback_porch.typ;

		dmode->vdisplay =  synaPanelTimings.vactive.typ;
		dmode->vsync_start = dmode->vdisplay + synaPanelTimings.vfront_porch.typ;
		dmode->vsync_end = dmode->vsync_start + synaPanelTimings.vsync_len.typ;
		dmode->vtotal = dmode->vsync_end + synaPanelTimings.vback_porch.typ;
		dmode->clock =  synaPanelTimings.pixelclock.typ ;

		drm_mode_set_name(dmode);
		dmode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;

		drm_mode_probed_add(connector, dmode);

		connector->display_info.bus_formats = &synaPanelInfo.outformat;
		connector->display_info.num_bus_formats = 1;
		connector->display_info.bpc = synaPanelInfo.bpp;
	}

	return 0;
}

static const struct drm_panel_funcs syna_panel_dsi_funcs = {
	.prepare = syna_panel_dsi_prepare,
	.unprepare = syna_panel_dsi_unprepare,
	.get_timings = syna_panel_dsi_get_timings,
	.get_modes = syna_panel_dsi_get_modes,
};

int syna_panel_dsi_init(struct platform_device *pdev)
{
	int err = 0;
	struct device *mipi_dev;

	mipi_dev = kmalloc(sizeof(struct device), GFP_KERNEL);
	mipi_dev->of_node = of_find_compatible_node(NULL, NULL, "syna,drm-dsi");

	if (!mipi_dev->of_node) {
		pr_err("panel node not found \n");
		return -ENODEV;
	}

	dsi_panel = devm_kzalloc(&pdev->dev, sizeof(struct drm_panel), GFP_KERNEL);
	if (!dsi_panel)
		return -ENOMEM;

	of_property_read_u32(mipi_dev->of_node, "hact", &synaPanelInfo.panel_timing.hact);
	of_property_read_u32(mipi_dev->of_node, "hfp", &synaPanelInfo.panel_timing.hfp);
	of_property_read_u32(mipi_dev->of_node, "hsa", &synaPanelInfo.panel_timing.hsa);
	of_property_read_u32(mipi_dev->of_node, "hbp", &synaPanelInfo.panel_timing.hbp);
	of_property_read_u32(mipi_dev->of_node, "vact", &synaPanelInfo.panel_timing.vact);
	of_property_read_u32(mipi_dev->of_node, "vfp", &synaPanelInfo.panel_timing.vfp);
	of_property_read_u32(mipi_dev->of_node, "vsa", &synaPanelInfo.panel_timing.vsa);
	of_property_read_u32(mipi_dev->of_node, "vbp", &synaPanelInfo.panel_timing.vbp);
	of_property_read_u32(mipi_dev->of_node, "pixclockKhz", &synaPanelInfo.panel_timing.pixelclock);

	of_property_read_u32(mipi_dev->of_node, "bits_per_pixel", &synaPanelInfo.bpp);
	of_property_read_u32(mipi_dev->of_node, "busformat", &synaPanelInfo.outformat);

	synaPanelInfo.cmdsize = of_property_count_u8_elems(mipi_dev->of_node, "command");

	if (synaPanelInfo.cmdsize ) {
		synaPanelInfo.cmd = devm_kzalloc(&pdev->dev, synaPanelInfo.cmdsize, GFP_KERNEL);
		if (!synaPanelInfo.cmd)
			return -ENOMEM;
	}

	of_property_read_u8_array(mipi_dev->of_node, "command", synaPanelInfo.cmd, synaPanelInfo.cmdsize);

	synaPanelTimings.hactive.min = synaPanelTimings.hactive.typ = \
					synaPanelTimings.hactive.max = synaPanelInfo.panel_timing.hact;
	synaPanelTimings.hfront_porch.min = synaPanelTimings.hfront_porch.typ = \
					synaPanelTimings.hfront_porch.max = synaPanelInfo.panel_timing.hfp;
	synaPanelTimings.hsync_len.min = synaPanelTimings.hsync_len.typ = \
					synaPanelTimings.hsync_len.max = synaPanelInfo.panel_timing.hsa;
	synaPanelTimings.hback_porch.min = synaPanelTimings.hback_porch.typ = \
					synaPanelTimings.hback_porch.max = synaPanelInfo.panel_timing.hbp;

	synaPanelTimings.vactive.min = synaPanelTimings.vactive.typ = \
					synaPanelTimings.vactive.max = synaPanelInfo.panel_timing.vact;
	synaPanelTimings.vfront_porch.min =  synaPanelTimings.vfront_porch.typ = \
					synaPanelTimings.vfront_porch.max = synaPanelInfo.panel_timing.vfp;
	synaPanelTimings.vsync_len.min = synaPanelTimings.vsync_len.typ = \
					synaPanelTimings.vsync_len.max = synaPanelInfo.panel_timing.vsa;
	synaPanelTimings.vback_porch.min = synaPanelTimings.vback_porch.typ = \
					synaPanelTimings.vback_porch.max = synaPanelInfo.panel_timing.vbp;

	synaPanelTimings.pixelclock.min = synaPanelTimings.pixelclock.max = \
					synaPanelTimings.pixelclock.typ = synaPanelInfo.panel_timing.pixelclock;

	synaPanelInfo.mipirst = devm_fwnode_get_index_gpiod_from_child(&pdev->dev,
									"mipirst", 0,
									of_fwnode_handle(mipi_dev->of_node),
									GPIOD_OUT_HIGH, "mipi");

	synaPanelInfo.mipibl = devm_fwnode_get_index_gpiod_from_child(&pdev->dev,
									"mipibl", 0,
									of_fwnode_handle(mipi_dev->of_node),
									GPIOD_OUT_HIGH, "mipibl");

	if (IS_ERR(synaPanelInfo.mipirst))
		pr_err("MIPI Rst GPIO not found \n");

	gpiod_set_value_cansleep(synaPanelInfo.mipirst, 0);
	gpiod_set_value_cansleep(synaPanelInfo.mipibl, 1);

	drm_panel_init(dsi_panel, mipi_dev, &syna_panel_dsi_funcs,
					DRM_MODE_CONNECTOR_DSI);

	drm_panel_add(dsi_panel);

	return err;
}

void syna_panel_dsi_deinit (void)
{
	kfree(synaPanelInfo.cmd);
}