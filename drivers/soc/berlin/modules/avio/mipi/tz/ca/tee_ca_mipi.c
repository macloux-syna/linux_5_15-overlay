// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2024 Synaptics Incorporated */

#include <linux/delay.h>
#include <linux/export.h>
#include <linux/firmware.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/mutex.h>

#include "tee_client_type.h"
#include "tee_client_api.h"
#include "tee_ca_vpp.h"
#include "tee_ca_common.h"
#include "hal_vpp_mipi.h"
#include "hal_mipi_wrap.h"
#include "hal_mipi_tz.h"

#define MIPITA_UUID	{0x1316a183, 0x894d, 0x43fe, \
			{0x98, 0x93, 0xbb, 0x94, 0x6a, 0xe1, 0x04, 0x3e}}

#define MIPITA_PATH	"ta/libmipi_dsi.ta"
#define MIPI_DSI_CONFIG			0x12
#define MIPI_DSI_PANEL_SND_CMD		0x13

typedef struct __MIPI_CA_CONTEXT__ {
	int initialized;
	TEEC_Session session;
	TEEC_Context tee_ctx;
	struct mutex mutex;
} MIPI_CA_CONTEXT;

static MIPI_CA_CONTEXT g_mipiCaContext;

static int mipi_load_ta(struct device *dev)
{
	const struct firmware *fw = NULL;
	const char *ta_img_path = MIPITA_PATH;
	TEEC_SharedMemory fw_shm = { 0, };
	TEEC_Parameter tee_param = { 0, };
	TEEC_Result result;
	int ret;

	mutex_lock(&g_mipiCaContext.mutex);
	if (g_mipiCaContext.initialized) {
		mutex_unlock(&g_mipiCaContext.mutex);
		return TEEC_SUCCESS;
	}

	ret = request_firmware(&fw, ta_img_path, dev);
	if (ret) {
		pr_err("faild req fw 0x%x %s\n", ret, ta_img_path);
		mutex_unlock(&g_mipiCaContext.mutex);
		return ret;
	}

	result = TEEC_InitializeContext(NULL, &g_mipiCaContext.tee_ctx);
	if (result != TEEC_SUCCESS) {
		pr_err("%s:%d: result:%x\n", __func__, __LINE__, result);
		goto free_fw;
	}

	fw_shm.size = ALIGN(fw->size, PAGE_SIZE);
	fw_shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
	result = TEEC_AllocateSharedMemory(&g_mipiCaContext.tee_ctx, &fw_shm);
	if (result || !fw_shm.buffer) {
		pr_err("can't allocate memory(%zu) for firmware loading: 0x%x\n",
				fw_shm.size, result);
		goto finalize_ctx;
	}
	memcpy(fw_shm.buffer, fw->data, fw->size);

	tee_param.memref.parent = &fw_shm;
	tee_param.memref.size = fw_shm.size;
	result = TEEC_RegisterTA(&g_mipiCaContext.tee_ctx, &tee_param,
				 TEEC_MEMREF_PARTIAL_INPUT);

	if (TEEC_ERROR_ACCESS_CONFLICT == result) {
		pr_warn("%s TA has been loaded\n", ta_img_path);
	} else if (result) {
		pr_err("can't register %s TA: 0x%08x\n", ta_img_path, result);
		goto free_shm;
	} else {
		pr_info("TA loaded sucessfully - %s\n", ta_img_path);
	}

	g_mipiCaContext.initialized = true;

free_shm:
	TEEC_ReleaseSharedMemory(&fw_shm);

finalize_ctx:
	if (result)
		TEEC_FinalizeContext(&g_mipiCaContext.tee_ctx);

free_fw:
	release_firmware(fw);
	mutex_unlock(&g_mipiCaContext.mutex);
	return result;
}

static int mipi_initialize(struct device *dev)
{
	TEEC_UUID CA_UUID = MIPITA_UUID;
	TEEC_Result result;

	if (!g_mipiCaContext.initialized)
		return -ENODEV;

	result = mipi_load_ta(dev);
	if (result != TEEC_SUCCESS) {
		pr_err("%s:%d: result:%x\n", __func__, __LINE__, result);
		return result;
	}

	result = TEEC_OpenSession(&g_mipiCaContext.tee_ctx,
			&g_mipiCaContext.session, &CA_UUID, TEEC_LOGIN_USER,
			NULL,   /* No connection data needed for TEEC_LOGIN_USER. */
			NULL,   /* No payload, and do not want cancellation. */
			NULL);

	if (result != TEEC_SUCCESS) {
		pr_err("%s:%d: result:%x\n", __func__, __LINE__, result);
	}

	return result;
}

static int mipi_pass_config_params(vpp_config_params vpp_config_param)
{
	TEEC_Operation operation;
	TEEC_SharedMemory shm;
	TEEC_Result result;

	if (!g_mipiCaContext.initialized)
		return -ENODEV;

	shm.size = PAGE_SIZE;
	shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;

	result = TEEC_AllocateSharedMemory(&g_mipiCaContext.tee_ctx,
					   &shm);

	if (result) {
		pr_err("%s:%d: TEEC_AllocateSharedMemory: result:%x\n",
				__func__, __LINE__, result);
		return result;
	}

	memcpy(shm.buffer, vpp_config_param.mipi_config_params,
	       sizeof(VPP_MIPI_LOAD_CONFIG));

	memset(&operation, 0, sizeof(TEEC_Operation));
	operation.params[0].memref.parent = &shm;
	operation.params[0].memref.size = shm.size;
	operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE,
						TEEC_VALUE_OUTPUT,
						TEEC_NONE,
						TEEC_NONE);

	operation.params[1].value.a = 0xdeadbeef;
	operation.started = 1;
	result = TEEC_InvokeCommand(&g_mipiCaContext.session, MIPI_DSI_CONFIG,
				    &operation, NULL);

	if ((result =! TEEC_SUCCESS) ||
	    (operation.params[1].value.a != TEEC_SUCCESS)) {
		pr_info("Mipi pass config info fail = %d\n", result);
	}

	TEEC_ReleaseSharedMemory(&shm);

	return result;
}

static int mipi_dsi_panel_send_cmd(unsigned int cmdsize, unsigned char *pcmd)
{
	TEEC_Operation operation;
	TEEC_SharedMemory shm;
	TEEC_Result result;

	if (!g_mipiCaContext.initialized)
		return -ENODEV;

	shm.size = PAGE_SIZE;
	shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;

	result = TEEC_AllocateSharedMemory(&g_mipiCaContext.tee_ctx,
					   &shm);

	if (result) {
		pr_err("%s:%d: TEEC_AllocateSharedMemory: result:%x\n",
				__func__, __LINE__, result);
		return result;
	}

	memcpy(shm.buffer, pcmd, cmdsize);

	memset(&operation, 0, sizeof(TEEC_Operation));
	operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE,
						TEEC_VALUE_INPUT,
						TEEC_VALUE_OUTPUT,
						TEEC_NONE);

	operation.params[0].memref.parent = &shm;
	operation.params[0].memref.size = shm.size;
	operation.params[1].value.a = cmdsize;
	operation.params[2].value.a = 0xdeadbeef;

	operation.started = 1;
	result = TEEC_InvokeCommand(&g_mipiCaContext.session,
				    MIPI_DSI_PANEL_SND_CMD,
				    &operation, NULL);

	if ((result =! TEEC_SUCCESS) ||
	    (operation.params[2].value.a != TEEC_SUCCESS)) {
		pr_info("Mipi panel cmd send fail = %d\n", result);
	}

	return result;
}

static void mipi_finalize(void)
{
	if (!g_mipiCaContext.initialized)
		return;

	g_mipiCaContext.initialized = 0;

	TEEC_CloseSession(&g_mipiCaContext.session);
	TEEC_FinalizeContext(&g_mipiCaContext.tee_ctx);
}

int TZ_mipi_dsi_panel_send_cmd(unsigned int cmdsize, unsigned char *pcmd)
{
	int ret;

	ret = mipi_dsi_panel_send_cmd(cmdsize, pcmd);
	if (ret)
		pr_info("panel send cmd failed: ret = 0x%x\n", ret);

	return ret;
}

int TZ_MIPI_Init(struct device *dev, vpp_config_params vpp_config_param)
{
	int ret;

	mutex_init(&g_mipiCaContext.mutex);

	ret = mipi_initialize(dev);
	if (ret) {
		pr_info("Initializing MIPI failed: ret = 0x%x\n", ret);
		return ret;
	}

	ret = mipi_pass_config_params(vpp_config_param);
	if (ret) {
		pr_info("Failed to pass MIPI config params to TA = 0x%x\n",
			ret);
	}

	return ret;
}

void TZ_MIPI_DeInit(void)
{
	mipi_finalize();
}

void wrap_mipi_initialize_fops(MIPI_DSI_FOPS *mipi_fops)
{
	mipi_fops->MIPI_DSI_Init = TZ_MIPI_Init;
	mipi_fops->MIPI_DSI_Send_Cmd = TZ_mipi_dsi_panel_send_cmd;
	mipi_fops->MIPI_DSI_DeInit = TZ_MIPI_DeInit;
}
