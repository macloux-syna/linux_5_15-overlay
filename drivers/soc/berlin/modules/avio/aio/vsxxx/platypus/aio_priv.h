/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) 2019-2020 Synaptics Incorporated */
#ifndef _AIO_PRIV_H_
#define _AIO_PRIV_H_

#include <linux/kernel.h>

#define AIO_MIC_CH_NUM (AIO_MIC_CH1 + 1)
#define MAX_DMIC_MODULES (2)

// PCM channel number configuration
#define MIN_CHANNELS    (2)
#define MAX_CHANNELS    (4)

// Default PCM channel map
#define PCM_DEF_CHL_MAP  (0xF)

// PDM to DHUB write size in bytes while in interleave mode & dummy byte inserted
#define PDM_DHUB_OCPF_CHUNK_SZ  (16)

#define AVIO_DEFAULT_GBL_CONFIG \
	_AVIO_GBL_REG_(RA_avioGbl_AVPLL_CTRL0, 0x00), \
	_AVIO_GBL_REG_(RA_avioGbl_APLL_WRAP0, 0x260), \
	_AVIO_GBL_REG_(RA_avioGbl_APLL_WRAP1, 0x260), \
	_AVIO_GBL_REG_(RA_avioGbl_AVPLLA_CLK_EN, 0x4D), \
	_AVIO_GBL_REG_(RA_avioGbl_CTRL1, 0x3C00),


#define IS_INVALID_ID(id)			((id) > AIO_ID_MAX_NUM)

#define	DMIC_CLK_TABLE_CONFIG	\
	{8000,  196608000, 95, 64, 0, 1, 48, 1}, \
	{11025, 180633600, 63, 256, 0, 1, 8, 1}, \
	{16000, 196608000, 47, 192, 0, 1, 8, 1}, \
	{22050, 180633600, 31, 128, 0, 1, 8, 1}, \
	{32000, 196608000, 23, 96, 0, 1, 8, 1}, \
	{44100, 180633600, 15, 64, 0, 1, 8, 1}, \
	{48000, 196608000, 15, 64, 0, 1, 8, 1}, \
	{64000, 196608000, 11, 48, 0, 1, 8, 1}, \
	{88200, 180633600, 7, 32, 0, 1, 8, 1}, \
	{96000, 196608000, 7, 32, 0, 1, 8, 1},

int aio_enablerxport_extra(void *hd, u32 id, bool enable);
#endif
