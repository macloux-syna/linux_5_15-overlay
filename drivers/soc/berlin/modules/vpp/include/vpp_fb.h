// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2023 Synaptics Incorporated */

#ifndef _VPP_FB_H_
#define _VPP_FB_H_

#include "vpp_mem.h"
//#include "vpp_defines.h"

typedef void (*CallbackHandler)(void* data);

typedef struct vpp_config_params {
	u32 frame_rate;
	u32 frame_size_ndx;
	int disp_res_id;
	int disp_out_type;
	u32 fb_count;
	int active_planes;
	int enable_frame_buf_copy;
	CallbackHandler callback;
	void *data;
}vpp_config_params;

int MV_VPP_FB_Init(VPP_MEM_LIST *mem_list);
void MV_VPP_FB_Deinit(void);
void MV_VPP_FB_ConfigParams(vpp_config_params* param);
void MV_VPP_FB_DisplayFrame(VPP_MEM *shm_handle, int x,
			int y, int w, int h, int stride);
void MV_VPP_FB_GetInputFrameSize(int *width, int *height);
int MV_VPP_FB_Config(void);

#endif //_VPP_FB_H_
