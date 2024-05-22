// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2024 Synaptics Incorporated */

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/tee_drv.h>

#include "tee_ca_vpp.h"
#include "hal_vpp_mipi.h"
#include "hal_mipi_tz.h"
#include "hal_mipi_wrap.h"

#define MIPITA_UUID	UUID_INIT(0x1316a183, 0x894d, 0x43fe, \
			0x98, 0x93, 0xbb, 0x94, 0x6a, 0xe1, 0x04, 0x3e)

#define MIPI_DSI_CONFIG			0x12
#define MIPI_DSI_PANEL_SND_CMD		0x13

typedef struct __MIPI_CA_CONTEXT__ {
	struct tee_ioctl_open_session_arg sess_arg;
	struct tee_context *context;
	int initialized;
	struct mutex mutex;
} MIPI_CA_CONTEXT;

static MIPI_CA_CONTEXT g_mipiCaContext;

static int optee_ctx_match(struct tee_ioctl_version_data *ver, const void *data)
{
	return (ver->impl_id == TEE_IMPL_ID_OPTEE);
}

static int mipi_initialize(struct device *dev)
{
	uuid_t TAMIPI_CA_UUID = MIPITA_UUID;
	int ret;

	mutex_lock(&g_mipiCaContext.mutex);
	if (g_mipiCaContext.initialized) {
		ret = 0;
		goto out;
	}

	g_mipiCaContext.context = tee_client_open_context(NULL,
							  optee_ctx_match,
							  NULL, NULL);

	if (IS_ERR(g_mipiCaContext.context)) {
		pr_err("fail to open mipi optee context\n");
		ret = -ENODEV;
		goto out;
	}

	memset(&g_mipiCaContext.sess_arg, 0, sizeof(g_mipiCaContext.sess_arg));
	memcpy(g_mipiCaContext.sess_arg.uuid, TAMIPI_CA_UUID.b,
	       TEE_IOCTL_UUID_LEN);

	g_mipiCaContext.sess_arg.clnt_login = TEE_IOCTL_LOGIN_PUBLIC;
	g_mipiCaContext.sess_arg.num_params = 0;

	ret = tee_client_open_session(g_mipiCaContext.context,
				      &g_mipiCaContext.sess_arg, NULL);

	if (ret < 0 || g_mipiCaContext.sess_arg.ret != 0) {
		pr_err("tee_client_open_session failed with code 0x%x origin 0x%x",
			ret, g_mipiCaContext.sess_arg.ret);
		tee_client_close_context(g_mipiCaContext.context);
		goto out;
	}

	g_mipiCaContext.initialized = true;

out:
	mutex_unlock(&g_mipiCaContext.mutex);
	return ret;
}

static int mipi_pass_config_params(vpp_config_params vpp_config_param)
{
	struct tee_ioctl_invoke_arg arg;
	struct tee_param param[2];
	struct tee_shm *shm;
	int ret;

	if (!g_mipiCaContext.initialized)
		return -ENODEV;

	memset(&arg, 0, sizeof(arg));
	arg.func = MIPI_DSI_CONFIG;
	arg.num_params = 2;
	arg.session = g_mipiCaContext.sess_arg.session;

	shm = tee_shm_alloc_kernel_buf(g_mipiCaContext.context,
				       PAGE_SIZE);

	if (IS_ERR(shm)) {
		pr_err("fail to allocate share memory: size %lx\n", PAGE_SIZE);
		return -ENOMEM;
	}

	memcpy(shm->kaddr, vpp_config_param.mipi_config_params,
	       sizeof(VPP_MIPI_LOAD_CONFIG));

	memset(param, 0, sizeof(param));
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_INPUT;
	param[0].u.memref.shm = shm;
	param[0].u.memref.size = shm->size;
	param[0].u.memref.shm_offs = 0;

	param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT;
	param[1].u.value.a = 0xdeadbeef;

	ret = tee_client_invoke_func(g_mipiCaContext.context, &arg, param);
	if (ret < 0 || arg.ret != 0 || param[1].u.value.a != 0 ) {
		pr_err("fail to invoke command(%x): ret = %x,"
				"TEE err: 0x%x\n", MIPI_DSI_CONFIG,
				ret, arg.ret);
	}

	tee_shm_free(shm);

	return ret;
}

static int mipi_dsi_panel_send_cmd(unsigned int cmdsize, unsigned char *pcmd)
{
	struct tee_ioctl_invoke_arg arg;
	struct tee_param param[3];
	struct tee_shm *shm;
	int ret;

	if (!g_mipiCaContext.initialized)
		return -ENODEV;

	memset(&arg, 0, sizeof(arg));
	arg.func = MIPI_DSI_PANEL_SND_CMD;
	arg.num_params = 3;
	arg.session = g_mipiCaContext.sess_arg.session;

	shm = tee_shm_alloc_kernel_buf(g_mipiCaContext.context,
				       cmdsize);

	if (IS_ERR(shm)) {
		pr_err("fail to allocate share memory: size %x\n", cmdsize);
		return -ENOMEM;
	}

	memcpy(shm->kaddr, pcmd, cmdsize);

	memset(param, 0, sizeof(param));
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_INPUT;
	param[0].u.memref.shm = shm;
	param[0].u.memref.size = shm->size;
	param[0].u.memref.shm_offs = 0;
	param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[1].u.value.a = cmdsize;
	param[2].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT;
	param[2].u.value.a = 0xdeadbeef;

	ret = tee_client_invoke_func(g_mipiCaContext.context, &arg, param);
	if (ret < 0 || arg.ret != 0 || param[2].u.value.a != 0 ) {
		pr_err("fail to invoke command(%x): ret = %x,"
				"TEE err: 0x%x\n", MIPI_DSI_PANEL_SND_CMD,
				ret, arg.ret);
	}

	tee_shm_free(shm);

	return ret;
}

static void mipi_finalize(void)
{
	if (!g_mipiCaContext.initialized)
		return;

	tee_client_close_session(g_mipiCaContext.context,
				 g_mipiCaContext.sess_arg.session);

	tee_client_close_context(g_mipiCaContext.context);
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
