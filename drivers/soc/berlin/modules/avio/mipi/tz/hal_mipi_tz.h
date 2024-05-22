// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2024 Synaptics Incorporated */

#ifndef _HAL_MIPI_TZ_H_
#define _HAL_MIPI_TZ_H_

#include "vpp_config.h"

int TZ_MIPI_Init(struct device *dev, vpp_config_params vpp_config_param);
int TZ_mipi_dsi_panel_send_cmd(unsigned int cmdsize, unsigned char *pcmd);
void TZ_MIPI_DeInit(void);

#endif
