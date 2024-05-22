// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2024 Synaptics Incorporated */

#include <linux/delay.h>
#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>

#include "hal_mipi_wrap.h"
#include "hal_mipi_ntz.h"

int NTZ_MIPI_Init(struct device *dev, vpp_config_params vpp_config_param)
{
	return 0;
}

int NTZ_mipi_dsi_panel_send_cmd(unsigned int cmdsize, unsigned char *pcmd)
{
	return 0;
}

void NTZ_MIPI_DeInit(void)
{
	return;
}

void wrap_mipi_initialize_fops(MIPI_DSI_FOPS *mipi_fops)
{
	mipi_fops->MIPI_DSI_Init = NTZ_MIPI_Init;
	mipi_fops->MIPI_DSI_Send_Cmd = NTZ_mipi_dsi_panel_send_cmd;
	mipi_fops->MIPI_DSI_DeInit = NTZ_MIPI_DeInit;
}
