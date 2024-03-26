// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Synaptics Incorporated
 *
 *
 * Author: Shanmugam Ramachandran <Shanmugam.Ramachandran@synaptics.com>
 *
 */

#include <linux/moduleparam.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/of.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_modes.h>
#include <video/videomode.h>

#include "drm_syna_drv.h"

#define PANEL_WIDTH 800
#define PANEL_HEIGHT 1280

static struct videomode panel_video_mode = {
	.pixelclock = PANEL_WIDTH*PANEL_HEIGHT*60,   /* pixelclock in Hz */

	.hactive = PANEL_WIDTH,
	.hfront_porch = 40,
	.hback_porch = 86,
	.hsync_len = 1,

	.vactive = PANEL_HEIGHT,
	.vfront_porch = 30,
	.vback_porch = 13,
	.vsync_len = 3,

	.flags = 0, /* display flags */
};


MODULE_LICENSE("Dual MIT/GPL");

static int syna_dsi_add_extra_modes(struct drm_connector *connector)
{
	struct drm_display_mode *dmode;
	struct videomode *vm = &panel_video_mode;
	int num_modes = 0;

	dmode = drm_mode_create(connector->dev);
	if (dmode) {
		dmode->hdisplay = vm->hactive;
		dmode->hsync_start = dmode->hdisplay + vm->hfront_porch;
		dmode->hsync_end = dmode->hsync_start + vm->hsync_len;
		dmode->htotal = dmode->hsync_end + vm->hback_porch;

		dmode->vdisplay = vm->vactive;
		dmode->vsync_start = dmode->vdisplay + vm->vfront_porch;
		dmode->vsync_end = dmode->vsync_start + vm->vsync_len;
		dmode->vtotal = dmode->vsync_end + vm->vback_porch;

		dmode->clock = vm->pixelclock / 1000;

		dmode->flags = 0;
		if (vm->flags & DISPLAY_FLAGS_HSYNC_HIGH)
			dmode->flags |= DRM_MODE_FLAG_PHSYNC;
		else if (vm->flags & DISPLAY_FLAGS_HSYNC_LOW)
			dmode->flags |= DRM_MODE_FLAG_NHSYNC;
		if (vm->flags & DISPLAY_FLAGS_VSYNC_HIGH)
			dmode->flags |= DRM_MODE_FLAG_PVSYNC;
		else if (vm->flags & DISPLAY_FLAGS_VSYNC_LOW)
			dmode->flags |= DRM_MODE_FLAG_NVSYNC;
		if (vm->flags & DISPLAY_FLAGS_INTERLACED)
			dmode->flags |= DRM_MODE_FLAG_INTERLACE;
		if (vm->flags & DISPLAY_FLAGS_DOUBLESCAN)
			dmode->flags |= DRM_MODE_FLAG_DBLSCAN;
		if (vm->flags & DISPLAY_FLAGS_DOUBLECLK)
			dmode->flags |= DRM_MODE_FLAG_DBLCLK;
		drm_mode_set_name(dmode);
		dmode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;

		drm_mode_probed_add(connector, dmode);
		num_modes++;
	}

	return num_modes;
}

static int syna_dsi_connector_helper_get_modes(struct drm_connector *connector)
{
	int num_modes;
	int max_width, max_height;

	max_width = panel_video_mode.hactive;
	max_height = panel_video_mode.vactive;

	num_modes = drm_add_modes_noedid(connector, max_width, max_height);
	num_modes += syna_dsi_add_extra_modes(connector);

	drm_mode_sort(&connector->probed_modes);

	DRM_DEBUG_DRIVER("[CONNECTOR:%d:%s] found %d modes\n",
			 connector->base.id, connector->name, num_modes);

	return num_modes;
}

static int syna_dsi_connector_helper_mode_valid(struct drm_connector *connector,
						struct drm_display_mode *mode)
{
	if (mode->flags & DRM_MODE_FLAG_INTERLACE)
		return MODE_NO_INTERLACE;
	else if (mode->flags & DRM_MODE_FLAG_DBLSCAN)
		return MODE_NO_DBLESCAN;

	if (panel_video_mode.hactive == mode->hdisplay &&
			panel_video_mode.vactive == mode->vdisplay)
		return MODE_OK;
	else
		return MODE_ERROR;
}

static void syna_dsi_connector_destroy(struct drm_connector *connector)
{
	struct syna_drm_private *dev_priv = connector->dev->dev_private;
	ENUM_VOUT_CONNECTOR vout_id =
		connector->connector_type == DRM_MODE_CONNECTOR_DVID ?
			  VOUT_CONNECTOR_HDMI : VOUT_CONNECTOR_DSI;

	if (!dev_priv) {
		DRM_ERROR("%s %d  device private is NULL!!\n",
			  __func__, __LINE__);
		return;
	}
	DRM_DEBUG_DRIVER("[CONNECTOR:%d:%s]\n",
			 connector->base.id, connector->name);

	drm_connector_cleanup(connector);

	kfree(connector);
	dev_priv->connector[vout_id] = NULL;
}

static void syna_dsi_connector_force(struct drm_connector *connector)
{
}

static struct drm_connector_helper_funcs syna_dsi_connector_helper_funcs = {
	.get_modes = syna_dsi_connector_helper_get_modes,
	.mode_valid = syna_dsi_connector_helper_mode_valid,
};

static const struct drm_connector_funcs syna_dsi_connector_funcs = {
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = syna_dsi_connector_destroy,
	.force = syna_dsi_connector_force,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
	.dpms = drm_helper_connector_dpms,
};

struct drm_connector *syna_dsi_connector_create(struct drm_device *dev)
{
	struct drm_connector *connector;

	connector = kzalloc(sizeof(*connector), GFP_KERNEL);
	if (!connector)
		return ERR_PTR(-ENOMEM);

	drm_connector_init(dev, connector, &syna_dsi_connector_funcs,
			   DRM_MODE_CONNECTOR_DSI);
	drm_connector_helper_add(connector, &syna_dsi_connector_helper_funcs);

	connector->dpms = DRM_MODE_DPMS_OFF;
	connector->interlace_allowed = false;
	connector->doublescan_allowed = false;
	connector->display_info.subpixel_order = SubPixelHorizontalRGB;

	syna_vpp_set_res_limit(dev, panel_video_mode.hactive,
			panel_video_mode.vactive);

	DRM_DEBUG_DRIVER("[CONNECTOR:%d:%s]\n", connector->base.id,
			 connector->name);

	return connector;
}
