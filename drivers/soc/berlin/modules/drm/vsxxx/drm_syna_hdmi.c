// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Synaptics Incorporated
 *
 *
 * Author: Lijun Fan <Lijun.Fan@synaptics.com>
 *
 */

#include <linux/moduleparam.h>
#include <linux/version.h>
#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_probe_helper.h>
#include "drm_syna_drv.h"
#include "syna_drm_priv.h"

#define MAX_EDID_BLOCKS 8 //Max EDID blocks supported by syna driver

static char preferred_mode_name[DRM_DISPLAY_MODE_LEN] = "\0";

module_param_string(hdmi_preferred_mode,
			preferred_mode_name, DRM_DISPLAY_MODE_LEN, 0444);

MODULE_PARM_DESC(hdmi_preferred_mode,
		 "Specify the preferred mode (if supported), e.g. 1280x1024.");
MODULE_LICENSE("Dual MIT/GPL");

static int syna_hdmi_get_edid (void *data, u8 *buf, unsigned int block, size_t len)
{
	VPP_HDMI_RAW_EDID *edid_buffer;
	int retVal;

	edid_buffer = kzalloc(sizeof(VPP_HDMI_RAW_EDID), GFP_KERNEL);
	if (!edid_buffer) {
		DRM_DEBUG_DRIVER("Memory could not be allocated for EDID\n");
		return -ENOMEM;
	}

	retVal = wrap_MV_VPPOBJ_GetHDMIRawEdid(edid_buffer);

	if (!edid_buffer->IsValid) {
		DRM_DEBUG_DRIVER("Invalid EDID\n");
		return -EINVAL;
	}

	if (block < MAX_EDID_BLOCKS) {
		memcpy(buf, edid_buffer->DataBuf + (block * 128), len);
	} else {
		DRM_DEBUG_DRIVER("Could no access requested block\n");
		return -EINVAL;
	}

	kfree(edid_buffer);
	return retVal;
}

static int syna_hdmi_connector_helper_get_modes(struct drm_connector *connector)
{
	int len = strlen(preferred_mode_name);
	struct edid *edid;
	int num_modes;

	if (len)
		DRM_DEBUG_DRIVER("detected hdmi_preferred_mode=%s\n",
				 preferred_mode_name);
	else
		DRM_DEBUG_DRIVER("no hdmi_preferred_mode\n");

	//Get raw edid from syna driver
	edid = drm_do_get_edid(connector, syna_hdmi_get_edid, NULL);
	DRM_DEBUG_DRIVER("Edid obtained and parsed \n");
	drm_connector_update_edid_property(connector, edid);
	num_modes = drm_add_edid_modes(connector, edid);
	kfree(edid);

	if (num_modes && len) {
		struct drm_display_mode *pref_mode_user = NULL;
		struct drm_display_mode *pref_mode_edid = NULL;

		struct drm_display_mode *mode, *t;
		list_for_each_entry_safe(mode, t, &connector->probed_modes, head) {
			if (mode->type & DRM_MODE_TYPE_PREFERRED) {
				DRM_DEBUG_DRIVER("preferred mode from EDID:%s\n", mode->name);
				pref_mode_edid = mode;
				mode->type &= ~DRM_MODE_TYPE_PREFERRED;
			}

			if ((pref_mode_user == NULL) && !strcmp(mode->name, preferred_mode_name)) {
				if ((drm_mode_vrefresh(mode) == 60) &&
					!(mode->flags & DRM_MODE_FLAG_INTERLACE)) {
					pref_mode_user = mode;
				}
			}
		}

		if (pref_mode_user)
			pref_mode_user->type |= DRM_MODE_TYPE_PREFERRED;
		else if (pref_mode_edid)
			pref_mode_edid->type |= DRM_MODE_TYPE_PREFERRED;
	}

	drm_mode_sort(&connector->probed_modes);
	DRM_DEBUG_DRIVER("[CONNECTOR:%d:%s] found %d modes\n",
					connector->base.id, connector->name, num_modes);

	return num_modes;
}

static int syna_hdmi_connector_helper_mode_valid(struct drm_connector *connector,
						struct drm_display_mode *mode)
{
	/*Interlaced and Doublescan mode currently not supported by VPP*/
	if (mode->flags & DRM_MODE_FLAG_INTERLACE)
		return MODE_NO_INTERLACE;
	else if (mode->flags & DRM_MODE_FLAG_DBLSCAN)
		return MODE_NO_DBLESCAN;

	if(mode->hdisplay > 3840 && mode->vdisplay > 2160)
		return MODE_ERROR;

	return MODE_OK;
}

static void syna_hdmi_connector_destroy(struct drm_connector *connector)
{
	struct syna_drm_private *dev_priv = connector->dev->dev_private;
	ENUM_VOUT_CONNECTOR vout_id =
		connector->connector_type == DRM_MODE_CONNECTOR_HDMIA ?
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

static void syna_hdmi_connector_force(struct drm_connector *connector)
{
}

static struct drm_connector_helper_funcs syna_hdmi_connector_helper_funcs = {
	.get_modes = syna_hdmi_connector_helper_get_modes,
	.mode_valid = syna_hdmi_connector_helper_mode_valid,
};

static const struct drm_connector_funcs syna_hdmi_connector_funcs = {
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = syna_hdmi_connector_destroy,
	.force = syna_hdmi_connector_force,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
	.dpms = drm_helper_connector_dpms,
};

struct drm_connector *syna_hdmi_connector_create(struct drm_device *dev)
{
	struct drm_connector *connector;

	connector = kzalloc(sizeof(*connector), GFP_KERNEL);
	if (!connector)
		return ERR_PTR(-ENOMEM);

	drm_connector_init(dev, connector, &syna_hdmi_connector_funcs,
				DRM_MODE_CONNECTOR_HDMIA);
	drm_connector_helper_add(connector, &syna_hdmi_connector_helper_funcs);

	connector->dpms = DRM_MODE_DPMS_OFF;
	connector->interlace_allowed = false;
	connector->doublescan_allowed = false;
	connector->display_info.subpixel_order = SubPixelHorizontalRGB;

	DRM_DEBUG_DRIVER("[CONNECTOR:%d:%s]\n", connector->base.id,
			 connector->name);

	return connector;
}
