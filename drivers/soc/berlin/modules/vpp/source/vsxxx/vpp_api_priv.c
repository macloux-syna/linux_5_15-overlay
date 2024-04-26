// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 */

#include "vpp_api.h"
#include "vpp_isr.h"
#include "hal_dhub_wrap.h"
#include "hal_vpp_wrap.h"
#include "avio_sub_module.h"

const int m_vinport_config[] = {
	/* PLANE_MAIN   */ CHAN_MAIN,
	/* PLANE_PIP	*/ CHAN_PIP,
	/* PLANE_GFX1   */ CHAN_GFX1,
	/* PLANE_GFX2   */ CHAN_GFX2,
	/* PLANE_AUX    */ CHAN_AUX,
	/* PLANE_OVP_EL */ CHAN_OVP_EL,
	/* PLANE_MAIN_EL*/ CHAN_MAIN_EL,
	/* PLANE_VMX	*/ CHAN_VMX,
};

int m_dv_config[] = {
	/* CHAN_MAIN    */ CPCB_1,
	/* CHAN_PIP     */ CPCB_1,
	/* CHAN_GFX1    */ CPCB_1,
	/* CHAN_GFX2    */ CPCB_INVALID,
	/* CHAN_AUX     */ CPCB_INVALID,
	/* CHAN_OVP_EL  */ CPCB_1,
	/* CHAN_MAIN_EL */ CPCB_1,
	/* CHAN_VMX     */ CPCB_1,
};

int m_zorder_config[] = {
#ifdef VPP_ENABLE_GFX_ON_TOP
	/* CHAN_MAIN    */ CPCB_ZORDER_1,
	/* CHAN_PIP     */ CPCB_ZORDER_2,
	/* CHAN_GFX1    */ CPCB_ZORDER_3,
#else
	/* CHAN_MAIN    */ CPCB_ZORDER_3,
	/* CHAN_PIP     */ CPCB_ZORDER_2,
	/* CHAN_GFX1    */ CPCB_ZORDER_1,
#endif
	/* CHAN_GFX2    */ CPCB_ZORDER_INVALID,
	/* CHAN_AUX     */ CPCB_ZORDER_INVALID,
	/* CHAN_OVP_EL  */ CPCB_ZORDER_INVALID,
	/* CHAN_MAIN_EL */ CPCB_ZORDER_INVALID,
	/* CHAN_VMX     */ CPCB_ZORDER_INVALID,
};

int m_voutport_config[] = {
	/* VOUT_HDMI  */ CPCB_1,
	/* VOUT_HD    */ CPCB_1,
	/* VOUT_SD    */ CPCB_1,
	/* VOUT_DSI   */ CPCB_INVALID,
};

static int VPP_Init_Recovery_fastlogo_ta(VPP_MEM_LIST *vpp_shm_list,
					vpp_config_params vpp_config_param)
{
	VPP_INIT_PARM vpp_init_parm;
	VPP_WIN_ATTR fb_attr;
	VPP_WIN fb_win;
	VPP_WIN disp_win;
	int width;
	int height;
	int disp_width;
	int disp_height;
	int res = 0;
	int planeID = PLANE_GFX1;
	/* 0 - Magic number, 1- heap handle which is not used FL ta as of now */
	unsigned int fl_init_param[2] = {VPP_CA_INIT_MAGIC_NUM, 0};
	VPP_DISP_OUT_PARAMS pdispParams[MAX_NUM_CPCBS];
	VPP_DISP_OUT_PARAMS dispParams;
	VPP_RESOLUTION_DESCRIPTION  ResDesc;
	VPP_MEM shm_handle;
	const int feature_cfg[MAX_NUM_FEATURE_CFG] = {VPP_FEATURE_HDMITX};

	//Allocate memory for TA heap memory manager
	shm_handle.size = SHM_SHARE_SZ;
	res = VPP_MEM_AllocateMemory(vpp_shm_list, VPP_MEM_TYPE_NS_NC,
				&shm_handle, GFP_KERNEL | __GFP_NOWARN);
	if (res != VPP_MEM_ERROR_TYPE_OK) {
		pr_info("VPP internal memory allocation: Not enough memory!!!!!!!!\n");
		return -ENOMEM;
	}

	//Initialize fastlogo TA
	vpp_init_parm.iHDMIEnable = 1;  //Set zero to disable flushcache in VPP TA
	vpp_init_parm.iVdacEnable = 0;
	vpp_init_parm.uiShmSize = SHM_SHARE_SZ;
	vpp_init_parm.uiShmPA = (uintptr_t)shm_handle.p_addr;

	res = wrap_MV_VPP_InitVPPS(TA_UUID_FASTLOGO, fl_init_param);
	if (res != MV_VPP_OK) {
		pr_err("wrap_MV_VPP_Init FAILED E[%d]\n", res);
		goto EXIT;
	}

	if (vpp_config_param.mipi_config_params)
		wrap_MV_VPP_LoadConfigTable(VOUT_DSI, 0, vpp_config_param.mipi_config_params);

	/* Retrieve following details from bootloader
	 * 1. dispout param - resid, display id/mode, TGID
	 * 2. Clock - Get pixel clock for resid got from BL.
	 * 3. Res Info - Get resInfo for resid got from BL.
	 * Default HDMI resID is DEFAULT_CPCB_1_RESOLUTION_ID/1080p@60.
	 * Default DSI resID is from BootLoader.
	 * Both (HDMI/DSI)of the above resID can be overridden by DTS
	 * entries/configuration.
	 */
	wrap_MV_VPPOBJ_GetDispOutParams(pdispParams, MAX_NUM_CPCBS * sizeof(VPP_DISP_OUT_PARAMS));

	/* Override the params from DTS if available */
	if (vpp_config_param.disp1_res_id  == -1) {
		dispParams.uiDispId = pdispParams[CPCB_1].uiDispId;
		dispParams.uiDisplayMode = pdispParams[CPCB_1].uiDisplayMode;
		dispParams.uiResId = IS_MIPI_ONLY_MODE(pdispParams[CPCB_1].uiDisplayMode) ?\
								pdispParams[CPCB_1].uiResId :\
								DEFAULT_CPCB_1_RESOLUTION_ID;
	} else {
		dispParams.uiDispId = (vpp_config_param.disp1_out_type == 0) ? VOUT_HDMI :\
									VOUT_DSI;
		dispParams.uiDisplayMode = vpp_config_param.display_mode;
		dispParams.uiResId = vpp_config_param.disp1_res_id;
	}

	if (dispParams.uiDispId == VOUT_DSI)
		wrap_MV_VPP_MIPI_Reset(0);

	vpp_config_param.disp1_res_id  = dispParams.uiResId;
	dispParams.uiBitDepth = vpp_config_param.disp1_bit_depth;
	dispParams.uiColorFmt = vpp_config_param.disp1_colorformat;
	dispParams.iPixelRepeat = 1;

	res = wrap_MV_VPPOBJ_GetResolutionDescription(vpp_config_param.disp1_res_id, &ResDesc);
	if (res != MV_VPP_OK) {
		pr_err("wrap_MV_VPPOBJ_GetResolutionDescription FAILED, error: 0x%x\n", res);
		res = MV_DISP_E_CFG;
		goto EXIT_DESTROY;
	}

	res = wrap_MV_VPPOBJ_Reset();
	if (res != MV_VPP_OK) {
		pr_err("wrap_MV_VPPOBJ_Reset FAILED, error: 0x%x\n", res);
		res = MV_DISP_E_RST;
		goto EXIT_DESTROY;
	}

	res = wrap_MV_VPPOBJ_Config(NULL, NULL, NULL, NULL, feature_cfg);
	if (res != MV_VPP_OK) {
		pr_err("wrap_MV_VPPOBJ_Config FAILED, error: 0x%x\n", res);
		res = MV_DISP_E_CFG;
		goto EXIT_DESTROY;
	}

	//Set the display resolution
	res = MV_VPP_SetDisplayResolution(CPCB_1, dispParams, 1);
	if (res != MV_VPP_OK) {
		pr_err("%s:%d: MV_VPP_SetDisplayResolution FAILED, error: 0x%x\n", __func__, __LINE__, res);
		goto EXIT_DESTROY;
	}

	//Get the width and height of FB
	MV_VPP_GetInputFrameSize(planeID, &width, &height);

	MV_VPP_GetOutResolutionSize(CPCB_1, &disp_width, &disp_height);
	fb_win.x = 0;
	fb_win.y = 0;
	fb_win.width  = width;
	fb_win.height = height;

	fb_attr.bgcolor = 0x801080; // black
	fb_attr.alpha  = 0xFFF;

	disp_win.x = 0;
	disp_win.y = 0;
	disp_win.width  = disp_width;
	disp_win.height = disp_height;

	res = wrap_MV_VPPOBJ_OpenDispWindow(planeID, &disp_win, &fb_attr);
	if (res != MV_VPP_OK) {
		pr_err("%s:%d OpenDispWindow FAILED, error: 0x%x\n", __func__, __LINE__, res);
		goto EXIT_DESTROY;
	}

#ifndef VPP_ENABLE_USE_SET_STILL_PICTURE
	res = wrap_MV_VPPOBJ_SetDisplayMode(planeID, DISP_FRAME);
	if (res != MV_VPP_OK) {
		pr_err("%s:%d SetDisplayMode FAILED, error: 0x%x\n", __func__, __LINE__, res);
		goto EXIT_DESTROY;
	}
#endif //VPP_ENABLE_USE_SET_STILL_PICTURE

	res = wrap_MV_VPPOBJ_SetRefWindow(planeID, &fb_win);
	if (res != MV_VPP_OK) {
		pr_err("%s:%d SetRefWindow FAILED, error: 0x%x\n", __func__, __LINE__, res);
		goto EXIT_DESTROY;
	}

	pr_info("MV_VPP_Init:Sucess: (libfastlogo.ta)\n");

	return 0;

EXIT_DESTROY:
	wrap_MV_VPPOBJ_Destroy();
EXIT:
	wrap_MV_VPP_DeInit();
	VPP_MEM_FreeMemory(vpp_shm_list, VPP_MEM_TYPE_NS_NC, &shm_handle);

	pr_err("MV_VPP_Init:Error: (libfastlogo.ta) - %d\n", res);

	return res;
}

static int VPP_Init_Recovery_vpp_ta(VPP_MEM_LIST *vpp_shm_list,
					vpp_config_params vpp_config_param)
{
	VPP_INIT_PARM vpp_init_parm;
	VPP_WIN_ATTR fb_attr;
	VPP_WIN fb_win;
	VPP_WIN disp_win;
	int width;
	int height;
	int disp_width;
	int disp_height;
	int res = 0;
	int planeID = PLANE_GFX1;
	VPP_DISP_OUT_PARAMS dispParams;
	VPP_MEM shm_handle;
	const int feature_cfg[MAX_NUM_FEATURE_CFG] = {VPP_FEATURE_HDMITX};
	VPP_HDMI_SINK_CAPS sinkCaps;

	//Allocate memory for TA heap memory manager
	shm_handle.size = SHM_SHARE_SZ;
	res = VPP_MEM_AllocateMemory(vpp_shm_list, VPP_MEM_TYPE_DMA,
				&shm_handle, GFP_KERNEL | __GFP_NOWARN);
	if (res != VPP_MEM_ERROR_TYPE_OK) {
		pr_info("VPP internal memory allocation: Not enough memory!!!!!!!!\n");
		return -ENOMEM;
	}

	//Initialize VPP TA
	vpp_init_parm.dev = vpp_shm_list->dev;
	vpp_init_parm.iHDMIEnable = 1;  //Set zero to disable flushcache in VPP TA
	vpp_init_parm.iVdacEnable = 0;
	vpp_init_parm.uiShmSize = SHM_SHARE_SZ;
	vpp_init_parm.uiShmPA = (uintptr_t)shm_handle.p_addr;

	dispParams.uiResId = (vpp_config_param.disp1_res_id == -1 ?
						 DEFAULT_CPCB_1_RESOLUTION_ID : vpp_config_param.disp1_res_id);
	dispParams.uiDispId = vpp_config_param.disp1_out_type;
	dispParams.uiDisplayMode = vpp_config_param.display_mode;
	dispParams.uiBitDepth = vpp_config_param.disp1_bit_depth;
	dispParams.uiColorFmt = vpp_config_param.disp1_colorformat;
	dispParams.iPixelRepeat = 1;

	res = wrap_MV_VPP_Init(&vpp_init_parm);
	if (res != MV_VPP_OK) {
		pr_err("wrap_MV_VPP_Init FAILED E[%d]\n", res);
		goto EXIT;
	}

	res = wrap_MV_VPPOBJ_Create(0, 0);
	if (res != MV_VPP_OK) {
		pr_err("wrap_MV_VPPOBJ_Create FAILED E[%d]\n", res);
		res = MV_DISP_E_CREATE;
		goto EXIT;
	}

	res = wrap_MV_VPPOBJ_Reset();
	if (res != MV_VPP_OK) {
		pr_err("wrap_MV_VPPOBJ_Reset FAILED, error: 0x%x\n", res);
		res = MV_DISP_E_RST;
		goto EXIT_DESTROY;
	}

#ifdef USE_DOLPHIN
	if (dispParams.uiDisplayMode == VPP_VOUT_DUAL_MODE_PIP)
		m_dv_config[CHAN_PIP] = CPCB_2;
#endif

	res = wrap_MV_VPPOBJ_Config(m_vinport_config, m_dv_config, m_zorder_config,
						m_voutport_config, feature_cfg);
	if (res != MV_VPP_OK) {
		pr_err("wrap_MV_VPPOBJ_Config FAILED, error: 0x%x\n", res);
		res = MV_DISP_E_CFG;
		goto EXIT_DESTROY;
	}

	if (vpp_config_param.mipi_config_params)
		wrap_MV_VPP_LoadConfigTable(VOUT_DSI, 0, vpp_config_param.mipi_config_params);

	wrap_MV_VPPOBJ_SetDispOutParams(&dispParams, CPCB_1);

	/* temporary workaround till dynamic format setting is working,
	   query to check if sink supports 4K, if it supports full-4K,
	   configure bootup resolution to be 4K30, else 1080p
	 */
	res = wrap_MV_VPPOBJ_GetHDMISinkCaps(&sinkCaps);
	if (res == MV_VPP_OK) {
		if (sinkCaps &
		    ((1<<VPP_HDMI_SINKCAP_BITMASK_FULL4K)|(1<<VPP_HDMI_SINKCAP_BITMASK_4K30))) {
			dispParams.uiResId = RES_4Kx2K30;
		}
	}
	pr_info("hdmi bootup to resId:%d colorfmt:%d bidepth:%d sinkcaps:%d\n",
		dispParams.uiResId,dispParams.uiColorFmt,dispParams.uiBitDepth,
		(res)?-1:sinkCaps);

	//Set the display resolution
	res = MV_VPP_SetDisplayResolution(CPCB_1, dispParams, 1);
	if (res != MV_VPP_OK) {
		pr_err("%s:%d: MV_VPP_SetDisplayResolution FAILED, error: 0x%x\n",
				__func__, __LINE__, res);
		goto EXIT_DESTROY;
	}

#ifdef USE_DOLPHIN
	if (vpp_config_param.display_mode == VPP_VOUT_DUAL_MODE_PIP) {
		dispParams.uiResId = vpp_config_param.disp2_res_id;
		dispParams.uiDispId = vpp_config_param.disp2_out_type;
		dispParams.uiDisplayMode = vpp_config_param.display_mode;
		dispParams.uiBitDepth = OUTPUT_BIT_DEPTH_8BIT;
		dispParams.uiColorFmt = OUTPUT_COLOR_FMT_RGB888;
		dispParams.iPixelRepeat = 1;
		wrap_MV_VPPOBJ_SetDispOutParams(&dispParams, CPCB_2);
		res = MV_VPP_SetDisplayResolution(CPCB_2, dispParams, 1);
		if (res != MV_VPP_OK) {
			pr_err("%s:%d: MV_VPP_SetDisplayResolution FAILED, error: 0x%x\n",
					__func__, __LINE__, res);
			goto EXIT_DESTROY;
		}
	}
#endif

	for (planeID = FIRST_PLANE; planeID < MAX_NUM_PLANES; planeID++) {
		if (!((1 << planeID) & vpp_config_param.active_planes))
			continue;

		MV_VPP_GetOutResolutionSize(m_dv_config[planeID], &disp_width, &disp_height);
		disp_win.x = 0;
		disp_win.y = 0;
		disp_win.width  = disp_width;
		disp_win.height = disp_height;

		//Get the width and height of FB
		MV_VPP_GetInputFrameSize(planeID, &width, &height);

		fb_win.x = 0;
		fb_win.y = 0;
		fb_win.width  = width;
		fb_win.height = height;

		fb_attr.bgcolor = 0x801080; // black
		fb_attr.alpha  = 0xFFF;
		fb_attr.globalAlphaFlag = (planeID != PLANE_GFX1 ? 0 : 1);

		res = wrap_MV_VPPOBJ_OpenDispWindow(planeID, &disp_win, &fb_attr);
		if (res != MV_VPP_OK) {
			pr_err("%s:%d OpenDispWindow(%d) FAILED, error: 0x%x\n", __func__,
					__LINE__, planeID, res);
			goto EXIT_DESTROY;
		}

#ifndef VPP_ENABLE_USE_SET_STILL_PICTURE
		res = wrap_MV_VPPOBJ_SetDisplayMode(planeID, DISP_FRAME);
		if (res != MV_VPP_OK) {
			pr_err("%s:%d SetDisplayMode FAILED, error: 0x%x\n",
				__func__, __LINE__, res);
			goto EXIT_DESTROY;
		}
#endif //VPP_ENABLE_USE_SET_STILL_PICTURE

		res = wrap_MV_VPPOBJ_SetRefWindow(planeID, &fb_win);
		if (res != MV_VPP_OK) {
			pr_err("%s:%d SetRefWindow FAILED, error: 0x%x\n",
				__func__, __LINE__, res);
			goto EXIT_DESTROY;
		}
    }

	pr_info("MV_VPP_Init:Sucess: (libfastlogo.ta)\n");

	return 0;

EXIT_DESTROY:
	wrap_MV_VPPOBJ_Destroy();
EXIT:
	wrap_MV_VPP_DeInit();
	VPP_MEM_FreeMemory(vpp_shm_list, VPP_MEM_TYPE_DMA, &shm_handle);

	pr_err("MV_VPP_Init:Error: (libfastlogo.ta) - %d\n", res);

	return res;
}

int wrap_VPP_Init_Recovery(VPP_MEM_LIST *shm_list,
			int is_ampless_boot, vpp_config_params vpp_config_param)
{
	int res = 0;

	if (wrap_MV_VPP_iSTeeEnabled()) {
		if (is_ampless_boot)
			res = VPP_Init_Recovery_vpp_ta(shm_list, vpp_config_param);
		else
			res = VPP_Init_Recovery_fastlogo_ta(shm_list, vpp_config_param);
	}

	return res;
}
