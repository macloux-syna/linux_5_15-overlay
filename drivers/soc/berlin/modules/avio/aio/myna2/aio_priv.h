/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) 2019-2020 Synaptics Incorporated */
#ifndef _AIO_PRIV_H_
#define _AIO_PRIV_H_

#include <linux/kernel.h>

#define AIO_MIC_CH_NUM (AIO_MIC_CH3 + 1)
#define MAX_DMIC_MODULES (4)

// PCM channel number configuration
#define MIN_CHANNELS    (2)
#define MAX_CHANNELS    (8)

// Default PCM channel map
#define PCM_DEF_CHL_MAP  (0xFF)

// PDM to DHUB write size in bytes while in interleave mode & dummy byte inserted
#define PDM_DHUB_OCPF_CHUNK_SZ  (32)

#define AVIO_DEFAULT_GBL_CONFIG \
	_AVIO_GBL_REG_(RA_avioGbl_SWPDWN_CTRL, 0x00), \
	_AVIO_GBL_REG_(RA_avioGbl_APLL0_WRAP, 0x260), \
	_AVIO_GBL_REG_(RA_avioGbl_APLL1_WRAP, 0x260), \
	_AVIO_GBL_REG_(RA_avioGbl_AVPLLA_CLK_EN, 0x7F), \
	_AVIO_GBL_REG_(RA_avioGbl_CTRL0, 0x2400),

#define IS_INVALID_ID(id)			((id) >= AIO_ID_I2S_MAX_NUM)

#define   MSK32AIO_APRKT_EN_SEC1                              0x00000001
#define   MSK32AIO_APRKT_EN_SEC2                              0x00000002
#define   MSK32AIO_APRKT_EN_SEC3                              0x00000004
#define   MSK32AIO_APRKT_EN_SEC4                              0x00000008
#define   MSK32AIO_APRKT_EN_SEC5                              0x00000010
#define   MSK32AIO_APRKT_EN_MIC21                             0x00000020
#define   MSK32AIO_APRKT_EN_MIC22                             0x00000040
#define   MSK32AIO_APRKT_EN_MIC23                             0x00000080
#define   MSK32AIO_APRKT_EN_MIC24                             0x00000100
#define   MSK32AIO_APRKT_EN_MIC25                             0x00000200
#define   MSK32AIO_APRKT_EN_PDM                               0x00000400

#define	DMIC_CLK_TABLE_CONFIG	\
	{8000,  294912000, 143, 96, 0, 1, 48, AIO_APLL_OUT}, \
	{11025, 270950400, 63, 256, 0, 1, 8, AIO_APLL_OUT}, \
	{16000, 294912000, 71, 48, 0, 1, 48, AIO_APLL_OUT}, \
	{22050, 270950400, 31, 128, 0, 1, 8, AIO_APLL_OUT}, \
	{32000, 294912000, 35, 144, 0, 1, 8, AIO_APLL_OUT}, \
	{44100, 270950400, 15, 64, 0, 1, 8, AIO_APLL_OUT}, \
	{48000, 294912000, 23, 96, 0, 1, 8, AIO_APLL_OUT}, \
	{64000, 294912000, 17, 72, 0, 1, 8, AIO_APLL_OUT}, \
	{88200, 270950400, 7, 32, 0, 1, 8, AIO_APLL_OUT}, \
	{96000, 294912000, 11, 48, 0, 1, 8, AIO_APLL_OUT},

enum aio_i2s_state {
	AIO_I2S_STATE_TX_MASTER = 0,
	AIO_I2S_STATE_TX_SLAVE = 1,
	AIO_I2S_STATE_TX_MS = 2,
	AIO_I2S_STATE_TX_DISABLED = 3,
	AIO_I2S_STATE_RX_SLAVE = 0,
	AIO_I2S_STATE_RX_MASTER = 1,
	AIO_I2S_STATE_RX_MS = 2,
	AIO_I2S_STATE_RX_DISABLED = 3
};

enum aio_aprkt_mode {
	AIO_I2S_APRKT_OFF,
	AIO_I2S_APRKT_ON_ABSO,
	AIO_I2S_APRKT_ON_ASAP,
	AIO_I2S_APRKT_ON_RELA
};

enum aio_ch_xid {
	AIO_ID_I2S1_TX = 0,
	AIO_ID_I2S1_RX = 1,
	AIO_ID_I2S2_TX = 2,
	AIO_ID_I2S2_RX = 3,
	AIO_ID_I2S3_TX = 4,
	AIO_ID_I2S3_RX = 5,
	AIO_ID_I2S4_TX = 6,
	AIO_ID_I2S4_RX = 7,
	AIO_ID_BCM     = 8,
	AIO_ID_PDM_IN  = 9,
	AIO_ID_I2S5_TX = 10,
	AIO_ID_I2S5_RX = 11,
	AIO_ID_I2S_MAX_NUM,
};

u32 aio_get_reg_base_addr(u32 id);
int aio_en_scr_str(void *hd, u32 id, bool en);
int aio_get_scr(void *hd, u32 id);
int aio_i2s_pause(void *hd, u32 id, bool pause, u32 pauseSampleCount);
int aio_set_sync_play(void *hd, u32 aprktEN, bool play);
int aio_set_extPortCfg(void *hd, u32 id, u32 aprktMode, bool enAprpr);
int aio_i2s_pause_status(void *hd);
int aio_get_bclk_xfeed_mode(void *hd, s32 i2sid);
#endif
