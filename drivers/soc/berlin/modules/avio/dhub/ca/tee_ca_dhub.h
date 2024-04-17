// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2020 Synaptics Incorporated
 *
 * Copyright (C) 2012 Marvell Technology Group Ltd.
 *		http://www.marvell.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef TEE_CA_DHUB_H
#define TEE_CA_DHUB_H
/** DHUB_API in trust zone
 */
#include <linux/types.h>

#include    "ctypes.h"
#include    "dhub_cmd.h"
/*structure define*/

#ifdef __cplusplus
extern "C" {
#endif

int DHUB_CA_Initialize(struct device *dev);
void DHUB_CA_EnableAutoPush(bool enable, bool useFastLogoTa);
void DHUB_CA_Finalize(void);

void DHUB_CA_Initialization(SIGN32 cpuId, UNSG32 dHubBaseAddr,
				UNSG32 hboSramAddr, int *pdhubHandle,
				int *dhub_config, SIGN32 numOfChans);
void DHUB_CA_ChannelClear(void *hdl, SIGN32 id, T64b cfgQ[]);

UNSG32 DHUB_CA_channel_write_cmd(void *hdl,	/*! Handle to HDL_dhub ! */
				SIGN32 id,	/*! Channel ID in $dHubReg ! */
				UNSG32 addr,	/*! CMD: buffer address ! */
				SIGN32 size,	/*! CMD: number of bytes to transfer ! */
				SIGN32 semOnMTU,	/*! CMD: semaphore operation at CMD/MTU (0/1) ! */
				SIGN32 chkSemId,	/*! CMD: non-zero to check semaphore ! */
				SIGN32 updSemId,	/*! CMD: non-zero to update semaphore ! */
				SIGN32 interrupt,	/*! CMD: raise interrupt at CMD finish ! */
				T64b cfgQ[],	/*! Pass NULL to directly update dHub, or
								 * Pass non-zero to receive programming sequence
								 * in (adr,data) pairs
								 * !
								 */
				UNSG32 *ptr	/*! Pass in current cmdQ pointer (in 64b word),
							 * & receive updated new pointer,
							 * Pass NULL to read from HW
							 * !
							 */
	);

void DHUB_CA_channel_generate_cmd(void *hdl,	/*! Handle to HDL_dhub ! */
				SIGN32 id,	/*! Channel ID in $dHubReg ! */
				UNSG32 addr,	/*! CMD: buffer address ! */
				SIGN32 size,	/*! CMD: number of bytes to transfer ! */
				SIGN32 semOnMTU,	/*! CMD: semaphore operation at CMD/MTU (0/1) ! */
				SIGN32 chkSemId,	/*! CMD: non-zero to check semaphore ! */
				SIGN32 updSemId,	/*! CMD: non-zero to update semaphore ! */
				SIGN32 interrupt,	/*! CMD: raise interrupt at CMD finish ! */
				SIGN32 *pData);

void DHUB_CA_semaphore_pop(void *hdl,	/*  Handle to HDL_semaphore */
				SIGN32 id,	/*  Semaphore ID in $SemaHub */
				SIGN32 delta	/*  Delta to pop as a consumer */

	);

void DHUB_CA_semaphore_clr_full(void *hdl,	/*  Handle to HDL_semaphore */
				SIGN32 id	/*  Semaphore ID in $SemaHub */

	);

UNSG32 DHUB_CA_semaphore_chk_full(void *hdl,	/*Handle to HDL_semaphore */
				SIGN32 id	/*Semaphore ID in $SemaHub
							 * -1 to return all 32b of the interrupt status
							 */
	);

void DHUB_CA_semaphore_intr_enable(void *hdl,	/*! Handle to HDL_semaphore ! */
				SIGN32 id,	/*! Semaphore ID in $SemaHub ! */
				SIGN32 empty,	/*! Interrupt enable for CPU at condition 'empty' ! */
				SIGN32 full,	/*! Interrupt enable for CPU at condition 'full' ! */
				SIGN32 almostEmpty,	/*! Interrupt enable for CPU at condition 'almostEmpty' ! */
				SIGN32 almostFull,	/*! Interrupt enable for CPU at condition 'almostFull' ! */
				SIGN32 cpu	/*! CPU ID (0/1/2) ! */

	);

void DHUB_CA_dhub2nd_channel_start_seq(void *hdl, SIGN32 id);
void DHUB_CA_dhub2nd_channel_clear_seq(void *hdl, SIGN32 id);
int DHUB_CA_BCM_SCHED_PushCmd(UNSG32 QID, UNSG32 *pCmd, UNSG32 *cfgQ);
void DHUB_CA_BCM_SCHED_SetMux(UNSG32 QID, UNSG32 TrigEvent);
void DHUB_CA_BCM_SCHED_GetEmptySts(UNSG32 QID, UNSG32 *EmptySts);

#ifdef __cplusplus
}
#endif
/** DHUB_API in trust zone
 */
#endif
/** ENDOFFILE: tee_ca_dhub.h
 */
