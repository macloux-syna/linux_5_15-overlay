// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2023 Synaptics Incorporated */

#ifndef _HAL_VPP_NTZ_H_
#define _HAL_VPP_NTZ_H_
/** hal_vpp_ntz.h for non TZ
 */

#include "hal_vpp.h"

#ifdef __cplusplus
extern "C" {
#endif

int NTZ_MV_VPP_Init(void);
int NTZ_MV_VPPOBJ_SetStillPicture(int planeID, void *pnew, void **pold);
int NTZ_MV_VPP_DeInit(void);
int NTZ_MV_VPPOBJ_SetPlaneMute(int planeID, int mute);
int NTZ_MV_VPPOBJ_Suspend(int enable);
int NTZ_MV_VPPOBJ_RecycleFrames(int planeID);
int NTZ_MV_VPPOBJ_Stop(void);
int NTZ_MV_VPPOBJ_Destroy(void);
int NTZ_MV_VPPOBJ_IsrHandler(unsigned int MsgId, unsigned int IntSts);
int NTZ_MV_VPPOBJ_GetCPCBOutputPixelClock(int resID, int *pixel_clock);
int NTZ_MV_VPP_LoadConfigTable(ENUM_VOUT_ID voutid, int Id, void *pConfig);
int NTZ_MV_VPPOBJ_DisplayFrame(int planeID, void *frame);
#ifdef __cplusplus
}
#endif
/** _HAL_VPP_NTZ_H_
 */
#endif // _HAL_VPP_NTZ_H_
/** ENDOFFILE: hal_vpp_ntz.h
 */
