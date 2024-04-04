// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Synaptics Incorporated
 *
 *
 * Author: Lijun Fan <Lijun.Fan@synaptics.com>
 *
 */

#if !defined(__DRM_SYNA_DRV_H__)
#define __DRM_SYNA_DRV_H__

#include <linux/version.h>
#include <linux/wait.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_mm.h>
#include <drm/drm_plane.h>
#include "syna_vpp.h"
#include "vpp_mem.h"
#include "syna_drm_priv.h"
#include "vpp_config.h"
#include "vpp_fb.h"

struct syna_gem_context;
enum syna_crtc_flip_status;
struct syna_flip_data;

struct syna_drm_private {
	struct drm_device *dev;

	/* initialised by syna_modeset_early_init */
	struct drm_plane *plane[MAX_NUM_PLANES];
	struct drm_crtc *crtc[MAX_CRTC];
	struct drm_connector *connector[MAX_CRTC];
	struct drm_encoder *encoder[MAX_CRTC];
	struct drm_panel *panel[MAX_CRTC];

	bool display_enabled;
	VPP_MEM_LIST *mem_list;
	vpp_config_params vpp_config_param;
};

struct syna_plane {
	struct drm_plane base;

	//plane_id
	uint32_t plane_id;
};
#define to_syna_plane(plane) container_of(plane, struct syna_plane, base)

struct syna_crtc {
	struct drm_crtc base;

	//cpcb_id
	uint32_t number;

	void __iomem *syna_reg;

	wait_queue_head_t flip_pending_wait_queue;

	/* Reuse the drm_device event_lock to protect these */
	atomic_t flip_status;
	struct drm_pending_vblank_event *flip_event;
	struct drm_framebuffer *old_fb;
	struct syna_flip_data *flip_data;
	bool flip_async;
};

#define to_syna_crtc(crtc) container_of(crtc, struct syna_crtc, base)
#define syna_framebuffer drm_framebuffer
#define to_syna_framebuffer(fb) (fb)

static inline u32 syna_drm_fb_cpp(struct drm_framebuffer *fb)
{
	return fb->format->cpp[0];
}

static inline u32 syna_drm_fb_format(struct drm_framebuffer *fb)
{
	return fb->format->format;
}

long syna_debugfs_get_hw_vsync_val(void);

struct drm_plane *syna_plane_create(struct drm_device *dev, int crtc_index,
					ENUM_PLANE_ID plane_id, enum drm_plane_type type);

struct drm_crtc *syna_crtc_create(struct drm_device *dev, uint32_t number,
					struct drm_plane *primary_plane);
void syna_crtc_set_vblank_enabled(struct drm_crtc *crtc, bool enable);
void syna_crtc_irq_handler(struct drm_crtc *crtc);

struct drm_connector *syna_dvi_connector_create(struct drm_device *dev);
struct drm_connector *syna_dsi_connector_create(struct drm_device *dev);
struct drm_connector *syna_lcdc_connector_create(struct drm_device *dev);

void drm_syna_encoder_suspend(struct syna_drm_private *dev_priv,
 							bool suspend);

int syna_modeset_early_init(struct syna_drm_private *dev_priv);
int syna_modeset_late_init(struct syna_drm_private *dev_priv);
void syna_modeset_late_cleanup(struct syna_drm_private *dev_priv);
int syna_modeset_createEntries(struct syna_drm_private *dev_priv);
void syna_read_config_priv(vpp_config_params *p_vpp_config_param);
#endif /* !defined(__DRM_SYNA_DRV_H__) */
