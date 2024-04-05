/*
// SPDX-License-Identifier: GPL-2.0
 * Copyright (C) 2023 Synaptics Incorporated
 *
 */
#ifndef __PANEL_SYNA
#define __PANEL_SYNA

#define MAX_PANELS           1
#define PANEL_MAX_WIDTH     3840
#define PANEL_MAX_HEIGHT    2160

void syna_panel_dsi_deinit(void);
int syna_dsi_panel_send_cmd (unsigned int cmdsize, unsigned char *pcmd);
#endif