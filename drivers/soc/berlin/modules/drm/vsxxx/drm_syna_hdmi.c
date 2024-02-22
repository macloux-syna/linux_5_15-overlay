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
#include "drm_syna_hdmi.h"
#include "vpp_cmd.h"
#include "vpp_api.h"
#include "syna_hdmi_config.h"

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
	struct syna_conn_hdmi *syna_hdmi = to_syna_conn_hdmi(connector);
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
	if ((edid == NULL) &&
		!syna_hdmi->syna_hdmi_conf.hdmiTxConfigFields.hpdHandlingEnabled) {
		/* load dummy edid as bootup without sink and no hpd handling enabled*/
		num_modes = drm_add_modes_noedid(connector, 1920, 1080);
		if (!len) {
			strncpy(preferred_mode_name, "1920x1080", DRM_DISPLAY_MODE_LEN);
			len = strlen(preferred_mode_name);
		}
	} else {
		/* even if hpd handling is disabled, if read EDID can be parsed on
		   startup, use the read edid instead of hardcoded */
		DRM_DEBUG_DRIVER("Edid obtained and parsed \n");
		drm_connector_update_edid_property(connector, edid);
		num_modes = drm_add_edid_modes(connector, edid);
		kfree(edid);
	}

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

static int syna_check_mode_enabled(int res_id, struct syna_conn_hdmi *syna_hdmi)
{
	int i;
	for (i = 1; i < syna_hdmi->count_format_supported; i++)
		if (res_id == syna_hdmi->supported_formats[i])
			return 0;

	return -1;
}

static int syna_hdmi_connector_helper_mode_valid(struct drm_connector *connector,
						struct drm_display_mode *mode)
{
	int res_id = MV_VPP_GetResIndex(mode->hdisplay, mode->vdisplay,
			mode->flags & DRM_MODE_FLAG_INTERLACE, mode->clock,
			drm_mode_vrefresh(mode));
	if (res_id < 0)
		return MODE_NOMODE;

	/*Interlaced and Doublescan mode currently not supported by VPP*/
	if (mode->flags & DRM_MODE_FLAG_INTERLACE)
		return MODE_NO_INTERLACE;
	else if (mode->flags & DRM_MODE_FLAG_DBLSCAN)
		return MODE_NO_DBLESCAN;

	if (syna_check_mode_enabled(res_id, to_syna_conn_hdmi(connector)))
		return MODE_NOMODE;

	return MODE_OK;
}

static void syna_hdmi_connector_destroy(struct drm_connector *connector)
{
	struct syna_conn_hdmi *syna_hdmi = to_syna_conn_hdmi(connector);
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

	if (syna_hdmi->syna_hdmi_conf.hdmiTxConfigFields.hpdHandlingEnabled)
		kthread_stop(syna_hdmi->hpd_monitor_task);

	kfree(syna_hdmi->supported_formats);

	drm_connector_cleanup(connector);

	kfree(syna_hdmi);
	dev_priv->connector[vout_id] = NULL;
}

static void syna_hdmi_connector_force(struct drm_connector *connector)
{
}

static enum drm_connector_status syna_hdmi_hotplug_detect(
                        struct drm_connector *connector,
                        bool force)
{
	unsigned char device_connected;
	struct syna_conn_hdmi *syna_hdmi = to_syna_conn_hdmi(connector);

	(void)force;
	/* read the HPD status from vpp hal, which is updated
	   based on HPD IRQ from dhub */

	if (syna_hdmi->syna_hdmi_conf.hdmiTxConfigFields.hpdHandlingEnabled)
		wrap_MV_VPPOBJ_GetHPDStatus(&device_connected, false);
	else
		device_connected = true; /*assume sink is connected always*/

	return (device_connected ? connector_status_connected :
					connector_status_disconnected);
}

static int syna_hdmi_hpd_monitor(void *param)
{
	struct drm_connector *connector = (struct drm_connector *)param;
	struct drm_device *dev = connector->dev;
	unsigned char activeHpdStatus;
	unsigned char hpdStatus;
	int retVal;

	connector->status = syna_hdmi_hotplug_detect(connector, false);
	activeHpdStatus = (connector->status == connector_status_connected) ? true : false;
	DRM_DEBUG_DRIVER("startup HDMI connection state : %d\n", activeHpdStatus);

	while (!kthread_should_stop()) {
		retVal = wrap_MV_VPP_WaitHdmiConnChange(&hpdStatus);
		if (retVal != 0)
			continue;

		if (hpdStatus != activeHpdStatus) {
			DRM_DEBUG_DRIVER("HDMI connection state changed to : %d\n", hpdStatus);
			activeHpdStatus = hpdStatus;
			connector->status = hpdStatus ? connector_status_connected :
							connector_status_disconnected;
			drm_kms_helper_hotplug_event(dev);
		}
	}

	return 0;
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
	.detect = syna_hdmi_hotplug_detect,
};

struct drm_connector *syna_hdmi_connector_create(struct drm_device *dev)
{
	struct syna_conn_hdmi *syna_hdmi;
	struct drm_connector *connector;
	int retVal;

	syna_hdmi = kzalloc(sizeof(*syna_hdmi), GFP_KERNEL);
	if (!syna_hdmi)
		return ERR_PTR(-ENOMEM);

	retVal = syna_hdmi_tx_read_config(syna_hdmi);
	if (retVal)
		return ERR_PTR(retVal);

	connector = &syna_hdmi->base;
	drm_connector_init(dev, connector, &syna_hdmi_connector_funcs,
				DRM_MODE_CONNECTOR_HDMIA);
	drm_connector_helper_add(connector, &syna_hdmi_connector_helper_funcs);

	connector->dpms = DRM_MODE_DPMS_OFF;
	connector->interlace_allowed = false;
	connector->doublescan_allowed = false;
	connector->display_info.subpixel_order = SubPixelHorizontalRGB;

	if (syna_hdmi->syna_hdmi_conf.hdmiTxConfigFields.hpdHandlingEnabled) {
		syna_hdmi->hpd_monitor_task = kthread_run(syna_hdmi_hpd_monitor, connector, "HDMI HPD monitor thread");
		if (!IS_ERR(syna_hdmi->hpd_monitor_task)) {
			connector->polled = DRM_CONNECTOR_POLL_HPD;
			DRM_DEBUG_DRIVER("syna_drm:enable HDMI HPD polling \n");
		}
	} else {
		DRM_DEBUG_DRIVER("hpd handling not enabled. assume connected always\n");
		connector->status = connector_status_connected;
	}

	DRM_DEBUG_DRIVER("[CONNECTOR:%d:%s]\n", connector->base.id,
			 connector->name);

	return connector;
}
