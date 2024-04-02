/* SPDX-License-Identifier: GPL-2.0*/
/*
 * Copyright (C) 2021 - 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ISPSS_API_DHUB_PRV_H__
#define __ISPSS_API_DHUB_PRV_H__

#define    semaphore_hdl                            ISPSS_semaphore_hdl
#define    semaphore_cfg                            ISPSS_semaphore_cfg
#define    semaphore_intr_enable                    ISPSS_semaphore_intr_enable
#define    semaphore_query                          ISPSS_semaphore_query
#define    semaphore_push                           ISPSS_semaphore_push
#define    semaphore_pop                            ISPSS_semaphore_pop
#define    semaphore_chk_empty                      ISPSS_semaphore_chk_empty
#define    semaphore_chk_full                       ISPSS_semaphore_chk_full
#define    dhub_wait_complete                       ISPSS_dhub_wait_complete
#define    semaphore_chk_almostEmpty                ISPSS_semaphore_chk_almostEmpty
#define    semaphore_chk_almostFull                 ISPSS_semaphore_chk_almostFull
#define    semaphore_clr_empty                      ISPSS_semaphore_clr_empty
#define    semaphore_clr_full                       ISPSS_semaphore_clr_full
#define    semaphore_clr_almostEmpty                ISPSS_semaphore_clr_almostEmpty
#define    semaphore_clr_almostFull                 ISPSS_semaphore_clr_almostFull
#define    hbo_hdl                                  ISPSS_hbo_hdl
#define    hbo_fifoCtl                              ISPSS_hbo_fifoCtl
#define    hbo_queue_cfg                            ISPSS_hbo_queue_cfg
#define    hbo_queue_enable                         ISPSS_hbo_queue_enable
#define    hbo_queue_clear                          ISPSS_hbo_queue_clear
#define    hbo_queue_busy                           ISPSS_hbo_queue_busy
#define    hbo_queue_clear_done                     ISPSS_hbo_queue_clear_done
#define    hbo_queue_read                           ISPSS_hbo_queue_read
#define    hbo_queue_write                          ISPSS_hbo_queue_write
#define    dhub_hdl                                 ISPSS_dhub_hdl
#define    dhub_semaphore                           ISPSS_dhub_semaphore
#define    dhub_hbo                                 ISPSS_dhub_hbo
#define    dhub_channel_cfg                         ISPSS_dhub_channel_cfg
#define    dhub_channel_enable                      ISPSS_dhub_channel_enable
#define    dhub_channel_clear                       ISPSS_dhub_channel_clear
#define    dhub_channel_flush                       ISPSS_dhub_channel_flush
#define    dhub_channel_busy                        ISPSS_dhub_channel_busy
#define    dhub_channel_pending                     ISPSS_dhub_channel_pending
#define    dhub_channel_clear_done                  ISPSS_dhub_channel_clear_done
#define    dhub_channel_write_cmd                   ISPSS_dhub_channel_write_cmd
#define    dhub_channel_generate_cmd                ISPSS_dhub_channel_generate_cmd
#define    diag_dhub_channel_big_write_cmd          ISPSS_diag_dhub_channel_big_write_cmd
#define    dhub_channel_big_write_cmd               ISPSS_dhub_channel_big_write_cmd
#define    dhub2d_hdl                               ISPSS_dhub2d_hdl
#define    dhub2d_channel_cfg                       ISPSS_dhub2d_channel_cfg
#define    dhub2d_channel_enable                    ISPSS_dhub2d_channel_enable
#define    dhub2d_channel_clear                     ISPSS_dhub2d_channel_clear
#define    dhub2nd_channel_cfg                      ISPSS_dhub2nd_channel_cfg
#define    dhub2nd_channel_enable                   ISPSS_dhub2nd_channel_enable
#define    dhub2nd_channel_clear                    ISPSS_dhub2nd_channel_clear
#define    dhub2d_channel_busy                      ISPSS_dhub2d_channel_busy
#define    dhub2d_channel_clear_done                ISPSS_dhub2d_channel_clear_done
#define    dhub_channel_enable_InverseScan_vppBcm   ISPSS_dhub_channel_enable_InverseScan_vppBcm
#define    BCM_SCHED_Open                           ISPSS_BCM_SCHED_Open
#define    BCM_SCHED_Close                          ISPSS_BCM_SCHED_Close
#define    BCM_SCHED_SetMux                         ISPSS_BCM_SCHED_SetMux
#define    BCM_SCHED_AutoPushCmd                    ISPSS_BCM_SCHED_AutoPushCmd
#define    dhub_channel_enable_InverseScan          ISPSS_dhub_channel_enable_InverseScan
#define    BCM_SCHED_PushCmd                        ISPSS_BCM_SCHED_PushCmd
#define    BCM_SCHED_GetEmptySts                    ISPSS_BCM_SCHED_GetEmptySts
#define    dhub2d_channel_clear_seq                 ISPSS_dhub2d_channel_clear_seq
#define    dhub2d_channel_start_seq                 ISPSS_dhub2d_channel_start_seq
#define    dhub2nd_channel_clear_seq                ISPSS_dhub2nd_channel_clear_seq
#define    dhub2nd_channel_start_seq                ISPSS_dhub2nd_channel_start_seq
#define    dhub_channel_clear_bcmbuf                ISPSS_dhub_channel_clear_bcmbuf
#define    dhub_channel_enable_bcmbuf               ISPSS_dhub_channel_enable_bcmbuf
#define    hbo_queue_clear_bcmbuf                   ISPSS_hbo_queue_clear_bcmbuf
#define    hbo_queue_enable_bcmbuf                  ISPSS_hbo_queue_enable_bcmbuf
#define    dhub2d_channel_enable_bcmbuf             ISPSS_dhub2d_channel_enable_bcmbuf
#define    dhub2d_channel_clear_bcmbuf              ISPSS_dhub2d_channel_clear_bcmbuf
#define    dhub2nd_channel_clear_bcmbuf             ISPSS_dhub2nd_channel_clear_bcmbuf
#define    dhub2d_channel_clear_seq_bcm             ISPSS_dhub2d_channel_clear_seq_bcm
#define    dhub_channel_clear_seq                   ISPSS_dhub_channel_clear_seq
#define    dhub2nd_channel_clear_seq_bcm            ISPSS_dhub2nd_channel_clear_seq_bcm

#define    HDL_semaphore                            ISPSS_HDL_semaphore
#define    HDL_hbo                                  ISPSS_HDL_hbo
#define    HDL_dhub                                 ISPSS_HDL_dhub
#define    HDL_dhub2d                               ISPSS_HDL_dhub2d
#define    BCM_SCHED_Q0                             ISPSS_BCM_SCHED_Q0
#define    BCM_SCHED_Q1                             ISPSS_BCM_SCHED_Q1
#define    BCM_SCHED_Q2                             ISPSS_BCM_SCHED_Q2
#define    BCM_SCHED_Q3                             ISPSS_BCM_SCHED_Q3
#define    BCM_SCHED_Q4                             ISPSS_BCM_SCHED_Q4
#define    BCM_SCHED_Q5                             ISPSS_BCM_SCHED_Q5
#define    BCM_SCHED_Q6                             ISPSS_BCM_SCHED_Q6
#define    BCM_SCHED_Q7                             ISPSS_BCM_SCHED_Q7
#define    BCM_SCHED_Q8                             ISPSS_BCM_SCHED_Q8
#define    BCM_SCHED_Q9                             ISPSS_BCM_SCHED_Q9
#define    BCM_SCHED_Q10                            ISPSS_BCM_SCHED_Q10
#define    BCM_SCHED_Q11                            ISPSS_BCM_SCHED_Q11
#define    BCM_SCHED_Q12                            ISPSS_BCM_SCHED_Q12
#define    BCM_SCHED_Q13                            ISPSS_BCM_SCHED_Q13
#define    BCM_SCHED_Q14                            ISPSS_BCM_SCHED_Q14
#define    BCM_SCHED_Q15                            ISPSS_BCM_SCHED_Q15
#define    BCM_SCHED_Q16                            ISPSS_BCM_SCHED_Q16
#define    BCM_SCHED_Q17                            ISPSS_BCM_SCHED_Q17
#define    BCM_SCHED_Q18                            ISPSS_BCM_SCHED_Q18
#define    ENUM_BCM_SCHED_QID                       ISPSS_ENUM_BCM_SCHED_QID
#define    BCM_SCHED_TRIG_CPCB0_VBI                 ISPSS_BCM_SCHED_TRIG_CPCB0_VBI
#define    BCM_SCHED_TRIG_CPCB1_VBI                 ISPSS_BCM_SCHED_TRIG_CPCB1_VBI
#define    BCM_SCHED_TRIG_CPCB2_VBI                 ISPSS_BCM_SCHED_TRIG_CPCB2_VBI
#define    BCM_SCHED_TRIG_CPCB0_VDE                 ISPSS_BCM_SCHED_TRIG_CPCB0_VDE
#define    BCM_SCHED_TRIG_CPCB1_VDE                 ISPSS_BCM_SCHED_TRIG_CPCB1_VDE
#define    BCM_SCHED_TRIG_CPCB2_VDE                 ISPSS_BCM_SCHED_TRIG_CPCB2_VDE
#define    BCM_SCHED_TRIG_AUD_PRIM                  ISPSS_BCM_SCHED_TRIG_AUD_PRIM
#define    BCM_SCHED_TRIG_AUD_SEC                   ISPSS_BCM_SCHED_TRIG_AUD_SEC
#define    BCM_SCHED_TRIG_AUD_HDMI                  ISPSS_BCM_SCHED_TRIG_AUD_HDMI
#define    BCM_SCHED_TRIG_AUD_SPDIF                 ISPSS_BCM_SCHED_TRIG_AUD_SPDIF
#define    BCM_SCHED_TRIG_AUD_MIC                   ISPSS_BCM_SCHED_TRIG_AUD_MIC
#define    BCM_SCHED_TRIG_VBI_VDE                   ISPSS_BCM_SCHED_TRIG_VBI_VDE
#define    BCM_SCHED_TRIG_DVI_VDE                   ISPSS_BCM_SCHED_TRIG_DVI_VDE
#define    BCM_SCHED_TRIG_SD_WRE                    ISPSS_BCM_SCHED_TRIG_SD_WRE
#define    BCM_SCHED_TRIG_SD_RDE                    ISPSS_BCM_SCHED_TRIG_SD_RDE
#define    BCM_SCHED_TRIG_SD_ERR                    ISPSS_BCM_SCHED_TRIG_SD_ERR
#define    BCM_SCHED_TRIG_NONE                      ISPSS_BCM_SCHED_TRIG_NONE
#define    ENUM_BCM_SCHED_TRIG_EVENT                ISPSS_ENUM_BCM_SCHED_TRIG_EVENT

#define    getDhubChannelInfo                       ISPSS_getDhubChannelInfo
#define    DhubInitialization                       ISPSS_DhubInitialization
#define    DhubChannelClear                         ISPSS_DhubChannelClear

#define     DHUB_channel_config                     ISPSS_DHUB_channel_config
#define     DHUB_TYPE                               ISPSS_DHUB_TYPE
#define     DHUB_TYPE_128BIT                        ISPSS_DHUB_TYPE_128BIT

#endif //__ISPSS_API_DHUB_PRV_H__
