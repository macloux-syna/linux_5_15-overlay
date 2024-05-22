// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2024 Synaptics Incorporated */

#ifndef _HAL_MIPI_WRAP_H_
#define _HAL_MIPI_WRAP_H_

#include "vpp_config.h"

typedef struct _MIPI_DSI_FOPS_ {
	int (*MIPI_DSI_Init)(struct device *dev,
			     vpp_config_params vpp_config_param);
	int (*MIPI_DSI_Send_Cmd)(unsigned int cmdsize, unsigned char *pcmd);
	void (*MIPI_DSI_DeInit)(void);
}MIPI_DSI_FOPS;

void wrap_mipi_initialize_fops(MIPI_DSI_FOPS *mipi_fops);
int wrap_mipi_initialize(struct device *dev,
			 vpp_config_params vpp_config_param);

int wrap_mipi_send_panel_cmd(unsigned int cmdsize, unsigned char *pcmd);

void wrap_mipi_deinit(void);

#endif //_HAL_MIPI_WRAP_H_
