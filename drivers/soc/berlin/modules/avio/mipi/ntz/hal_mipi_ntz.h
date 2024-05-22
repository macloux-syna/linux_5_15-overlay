// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2024 Synaptics Incorporated */

#ifndef _HAL_MIPI_NTZ_H_
#define _HAL_MIPI_NTZ_H_

#include "vpp_config.h"

int NTZ_MIPI_Init(struct device *dev, vpp_config_params vpp_config_param);
int NTZ_mipi_dsi_panel_send_cmd(unsigned int cmdsize, unsigned char *pcmd);
void NTZ_MIPI_DeInit(void);

#endif
