// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2024 Synaptics Incorporated */

#include <linux/delay.h>
#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>

#include "vpp_config.h"
#include "hal_mipi_wrap.h"

static MIPI_DSI_FOPS mipi_fops;

int wrap_mipi_initialize(struct device *dev,
			 vpp_config_params vpp_config_param)
{

	wrap_mipi_initialize_fops(&mipi_fops);

	if (mipi_fops.MIPI_DSI_Init)
		return mipi_fops.MIPI_DSI_Init(dev, vpp_config_param);

	return -ENODEV;
}
EXPORT_SYMBOL(wrap_mipi_initialize);

int wrap_mipi_send_panel_cmd(unsigned int cmdsize, unsigned char *pcmd)
{
	if (mipi_fops.MIPI_DSI_Send_Cmd)
		return mipi_fops.MIPI_DSI_Send_Cmd(cmdsize, pcmd);

	return -ENODEV;
}
EXPORT_SYMBOL(wrap_mipi_send_panel_cmd);

void wrap_mipi_deinit(void)
{
	if (mipi_fops.MIPI_DSI_DeInit)
		mipi_fops.MIPI_DSI_DeInit();
}
EXPORT_SYMBOL(wrap_mipi_deinit);
