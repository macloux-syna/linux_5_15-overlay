/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2023, Synaptics Incorporated
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/highmem.h>
#include <linux/firmware.h>

#include "vpu_common.h"
#include "vpu_fw_data.h"
#include "vpu_fw.h"

static const TEEC_UUID TAVmeta_UUID = { 0x1316a183, 0x894d, 0x43fe,
	{0x98, 0x93, 0xbb, 0x94, 0x6a, 0xe1, 0x03, 0xf0}
};

static int fw_debug = 0;
module_param(fw_debug, int, 0660);
MODULE_PARM_DESC(fw_debug, "Firmware debug level");

int vpu_open_tz_session(struct syna_vpu_dev *vpu, TEEC_Context * tee_ctx)
{
	TEEC_Operation operation = { 0 };
	int ret;

	operation.paramTypes =
	    TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_OUTPUT, TEEC_NONE,
			     TEEC_NONE);

	operation.params[0].value.a = vpu->variant->hw_type;
	operation.params[0].value.b = 2;
	operation.params[1].value.a = 0xdeadbeef;

	/* Decoder and Encoder would open its own session */
	ret = TEEC_OpenSession(tee_ctx, &vpu->s_session,
			       &TAVmeta_UUID, TEEC_LOGIN_USER,
			       NULL, &operation, NULL);
	if (ret || operation.params[1].value.a) {
		pr_err("[vpu]can't start a new TEE session ret=0x%08x 0x%08x\n",
		       ret, operation.params[1].value.a);
		ret = -ENXIO;
	}
	return ret;
}

int syna_vpu_get_hwdata(TEEC_Context * tee_ctx, struct syna_vpu_dev *vpu)
{
	struct syna_vpu_fw_data *fw_data = &vpu->fw_data;
	TEEC_SharedMemory shm = { 0, };
	TEEC_Operation oper = { 0, };
	int ret = 0;

	shm.size = ALIGN(sizeof(struct syna_vpu_fw_data), SZ_4K);
	shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
	/* FIXME: we could re-use the one for firmware loading */
	ret = TEEC_AllocateSharedMemory(tee_ctx, &shm);
	if (ret || !shm.buffer) {
		pr_err("can't allocate firmware memory\n");
		ret = -ENOMEM;
		goto failed;
	}
	oper.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_OUTPUT,
					   TEEC_NONE, TEEC_NONE);

	oper.params[0].value.a = (uintptr_t) shm.phyAddr;
	oper.params[0].value.b = shm.size;
	oper.params[1].value.a = 0x0;
	oper.params[1].value.b = 0xdeadbeef;

	ret =
	    TEEC_InvokeCommand(&vpu->s_session, VMETA_GET_CONST_V2, &oper,
			       NULL);
	if (ret || oper.params[1].value.b) {
		pr_err("failed to command VMETA TA: 0x%08x, 0x%08x\n",
		       ret, oper.params[1].value.b);
		ret = -EIO;
		goto failed_tee;
	}

	memcpy(fw_data, shm.buffer, sizeof(struct syna_vpu_fw_data));
	TEEC_ReleaseSharedMemory(&shm);
	fw_data->fw_api_ver = oper.params[1].value.a;
	return 0;

failed_tee:
	TEEC_ReleaseSharedMemory(&shm);
failed:
	return ret;
}

int syna_vpu_set_log_level(TEEC_Session * session)
{
	static int cur_level = 0;
	TEEC_Operation oper = { 0, };
	int ret = 0;

	if (cur_level == fw_debug)
		return 0;

	oper.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					   TEEC_VALUE_OUTPUT,
					   TEEC_NONE, TEEC_NONE);
	oper.params[0].value.a = fw_debug;
	oper.params[1].value.a = 0xdeadbeef;

	ret = TEEC_InvokeCommand(session, VPU_SET_DEBUG_LEVEL_V2, &oper, NULL);
	if (ret)
		return ret;

	cur_level = fw_debug;
	return 0;
}

/* tz_vpu_create_instance
	IN params0: ctrlshm addr, ctrlshm size
	OUT params1: result
*/
static int tz_vpu_create_instance(int cmd, struct syna_vcodec_ctx *ctx)
{
	struct syna_vpu_dev *vpu = ctx->vpu;
	struct syna_vpu_ctrl *ctrl;
	TEEC_Operation operation = { 0 };
	int ret;

	operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
						TEEC_VALUE_OUTPUT,
						TEEC_NONE, TEEC_NONE);

	operation.params[0].value.a = (uintptr_t) ctx->ctrl_shm.phyAddr;
	operation.params[0].value.b = ctx->ctrl_shm.size;
	operation.params[1].value.a = 0xdeadbeef;

	ret = TEEC_InvokeCommand(&vpu->s_session, cmd, &operation, NULL);
	if (ret || operation.params[1].value.a) {
		pr_err("can't create inst cmd %d ret:0x%x(0x%x)",
		       cmd, ret, operation.params[1].value.a);
		ret = operation.params[2].value.a;
	} else {
		ctrl = ctx->ctrl_shm.buffer;
		ctx->ctrl_shm_vaddr = ctrl->ctrl_buf.vaddr64;
	}
	return ret;
}

/**tz_vpu_stream_cmd1
 * IN params[0] ctrl tzva, ctrl size
 * OUT prams[1] result
*/
static int tz_vpu_stream_cmd1(int cmd, struct syna_vcodec_ctx *ctx)
{
	struct syna_vpu_dev *vpu = ctx->vpu;
	TEEC_Operation operation = { 0 };
	int ret = 0;

	operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
						TEEC_VALUE_OUTPUT,
						TEEC_NONE, TEEC_NONE);
	operation.params[0].value.a = ctx->ctrl_shm_vaddr;
	operation.params[0].value.b =
	    ALIGN(sizeof(struct syna_vpu_ctrl), SZ_4K);
	operation.params[1].value.a = 0xdeadbeef;

	ret = TEEC_InvokeCommand(&vpu->s_session, cmd, &operation, NULL);
	if (ret) {
		pr_err("stmcmd %d failed: %08x\n", cmd, ret);
		operation.params[1].value.a = ret;
	}

	return operation.params[1].value.a;
}

/**tz_vpu_stream_cmd2
 * IN  params[0] ctrl
 * IN  params[1] strm cfg
 * OUT prams[2] result
*/
static int tz_vpu_stream_cmd2(int cmd, struct syna_vcodec_ctx *ctx)
{
	struct syna_vpu_dev *vpu = ctx->vpu;
	TEEC_Operation operation = { 0 };
	int ret = 0;

	operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
						TEEC_VALUE_INPUT,
						TEEC_VALUE_OUTPUT, TEEC_NONE);

	operation.params[0].value.a = ctx->ctrl_shm_vaddr;
	operation.params[0].value.b =
	    ALIGN(sizeof(struct syna_vpu_ctrl), SZ_4K);
	operation.params[1].value.a = (uintptr_t) ctx->cfg_shm.phyAddr;
	operation.params[1].value.b = ctx->cfg_shm.size;
	operation.params[2].value.a = 0xdeadbeef;

	ret = TEEC_InvokeCommand(&vpu->s_session, cmd, &operation, NULL);

	if (ret || operation.params[2].value.a)
		return ret;

	return operation.params[2].value.a;
}

/** tz_vpu_buffer_cmd
 * IN  params[0] ctrl
 * IN  params[1] type, index
 * OUT prams[2] result
*/
static int tz_vpu_buffer_cmd(int cmd, struct syna_vcodec_ctx *ctx,
			     uint32_t type, uint32_t index)
{
	struct syna_vpu_dev *vpu = ctx->vpu;
	TEEC_Operation operation = { 0 };
	int ret;

	operation.paramTypes =
	    TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT,
			     TEEC_VALUE_OUTPUT, TEEC_NONE);

	operation.params[0].value.a = ctx->ctrl_shm_vaddr;
	operation.params[0].value.b =
	    ALIGN(sizeof(struct syna_vpu_ctrl), SZ_4K);
	operation.params[1].value.a = type;
	operation.params[1].value.b = index;
	operation.params[2].value.a = 0xdeadbeef;

	ret = TEEC_InvokeCommand(&vpu->s_session, cmd, &operation, NULL);

	if (ret || operation.params[2].value.a) {
		pr_err("can't put %d buffer into fw queue(type %d): 0x%x(0x%x)",
		       index, type, ret, operation.params[2].value.a);
		ret = operation.params[2].value.a;
	}
	return ret;
}

/*******************************************************************
 * VPU Common API
*******************************************************************/

int syna_vpu_open(struct syna_vpu_dev *vpu, u32 memid, u32 size)
{
	TEEC_Operation operation = { 0 };
	int ret;

	operation.paramTypes =
	    TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_OUTPUT, TEEC_NONE,
			     TEEC_NONE);

	operation.params[0].value.a = memid;
	operation.params[0].value.b = size;
	operation.params[1].value.a = 0xdeadbeef;

	ret = TEEC_InvokeCommand(&vpu->s_session, VMETA_OPEN_V2, &operation,
				 NULL);
	if (ret || operation.params[1].value.a) {
		pr_err("[vpu]failed to initialize VPU HW: 0x%08x, 0x%08x\n",
		       ret, operation.params[1].value.a);
		ret = -EIO;
	}
	return ret;
}

int syna_vpu_close(struct syna_vpu_dev *vpu)
{
	TEEC_Operation operation = { 0 };
	int ret;

	operation.paramTypes =
	    TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_OUTPUT, TEEC_NONE,
			     TEEC_NONE);

	operation.params[0].value.a = 0xdeadbeef;
	ret = TEEC_InvokeCommand(&vpu->s_session, VMETA_CLOSE_V2, &operation,
				 NULL);
	return ret;
}

int syna_vpu_register_buf(struct syna_vcodec_ctx *ctx, u32 buftype, u32 idx)
{
	return tz_vpu_buffer_cmd(VPU_REG_BUF_V2, ctx, buftype, idx);
}

int syna_vpu_unregister_buf(struct syna_vcodec_ctx *ctx, u32 buftype, u32 idx)
{
	return tz_vpu_buffer_cmd(VPU_UNREG_BUF_V2, ctx, buftype, idx);
}

/*********************************************************
	VDEC TA APIS
*********************************************************/
int syna_vdec_create_instance(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_create_instance(VDEC_CREATE_INSTANCE_V2, ctx);
}

int syna_vdec_destroy_instance(struct syna_vcodec_ctx *ctx)
{
	int ret;

	ret = tz_vpu_stream_cmd1(VDEC_DESTROY_INSTANCE_V2, ctx);
	clear_bit(SYNA_VPU_STATUS_SET_FMT, &ctx->status);

	return ret;
}

int syna_vdec_configure_stream(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd2(VDEC_CONFIG_STREAM_V2, ctx);
}

int syna_vdec_close_stream(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd1(VDEC_RESET_INSTANCE_V2, ctx);
}

int syna_vdec_stream_switch_in(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd1(VDEC_STREAM_SWITCH_IN_V2, ctx);
}

int syna_vdec_stream_switch_out(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd1(VDEC_STREAM_SWITCH_OUT_V2, ctx);
}

int syna_vdec_decode_stream(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd1(VDEC_DECODE_STREAM_V2, ctx);
}

int syna_vdec_hw_abort(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd1(VDEC_ABORT_HW_V2, ctx);
}

int syna_vdec_push_disp_buf(struct syna_vcodec_ctx *ctx, uint32_t index)
{
	return tz_vpu_buffer_cmd(VDEC_PUSH_BUFFER_V2, ctx,
				 SYNA_VPU_FW_VDEC_DISP, index);
}

int syna_vdec_push_strm_buf(struct syna_vcodec_ctx *ctx, uint32_t index)
{
	return tz_vpu_buffer_cmd(VDEC_PUSH_BUFFER_V2, ctx,
				 SYNA_VPU_FW_VDEC_STRM, index);
}

int syna_vdec_push_ref_buf(struct syna_vcodec_ctx *ctx, uint32_t index)
{
	return tz_vpu_buffer_cmd(VDEC_PUSH_BUFFER_V2, ctx,
				 SYNA_VPU_FW_VDEC_REF, index);
}

int syna_vdec_pop_disp_buf(struct syna_vcodec_ctx *ctx, uint32_t * index)
{
	struct syna_vpu_dev *vpu = ctx->vpu;
	TEEC_Operation operation = { 0 };
	int ret;

	operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
						TEEC_VALUE_INOUT,
						TEEC_VALUE_OUTPUT, TEEC_NONE);

	operation.params[0].value.a = ctx->ctrl_shm_vaddr;
	operation.params[0].value.b =
	    ALIGN(sizeof(struct syna_vpu_ctrl), SZ_4K);
	operation.params[1].value.a = SYNA_VPU_FW_VDEC_DISP;
	operation.params[1].value.b = 0xdeadbeef;
	operation.params[2].value.a = 0xdeadbeef;

	ret = TEEC_InvokeCommand(&vpu->s_session, VDEC_POP_BUFFER_V2,
				 &operation, NULL);

	if (ret || operation.params[2].value.a) {
		pr_debug("can't pop buffer from fw queue(type %d): 0x%x(0x%x)",
			 SYNA_VPU_FW_VDEC_DISP, ret,
			 operation.params[2].value.a);
		ret = operation.params[2].value.a;
	}
	*index = operation.params[1].value.b;
	return ret;
}

/*********************************************************
	VENC TA APIS
*********************************************************/
int syna_venc_create_instance(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_create_instance(VENC_CREATE_INSTANCE_V2, ctx);
}

int syna_venc_destroy_instance(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd1(VENC_DESTROY_INSTANCE_V2, ctx);
}

int syna_venc_configure_stream(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd2(VENC_CONFIG_STREAM_V2, ctx);
}

int syna_venc_stream_switch_in(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd1(VENC_STREAM_SWITCH_IN_V2, ctx);
}

int syna_venc_stream_switch_out(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd1(VENC_STREAM_SWITCH_OUT_V2, ctx);
}

int syna_venc_close_stream(struct syna_vcodec_ctx *ctx)
{
	int ret;

	if (!test_bit(SYNA_VPU_STATUS_SET_FMT, &ctx->status))
		return 0;

	ret = tz_vpu_stream_cmd1(VENC_CLOSE_STREAM_V2, ctx);
	if (ret) {
		return ret;
	}

	clear_bit(SYNA_VPU_STATUS_SET_FMT, &ctx->status);
	return ret;
}

int syna_venc_encode_stream(struct syna_vcodec_ctx *ctx)
{
	return tz_vpu_stream_cmd1(VENC_ENCODE_STREAM_V2, ctx);
}

int syna_venc_push_video_buf(struct syna_vcodec_ctx *ctx, uint32_t index)
{
	return tz_vpu_buffer_cmd(VENC_PUSH_BUFFER_V2, ctx,
				 SYNA_VPU_FW_VENC_VID, index);
}

int syna_venc_push_es_buf(struct syna_vcodec_ctx *ctx, uint32_t index)
{
	return tz_vpu_buffer_cmd(VENC_PUSH_BUFFER_V2, ctx,
				 SYNA_VPU_FW_VENC_STRM, index);
}

int syna_venc_push_ref_buf(struct syna_vcodec_ctx *ctx, uint32_t index)
{
	return tz_vpu_buffer_cmd(VENC_PUSH_BUFFER_V2, ctx,
				 SYNA_VPU_FW_VENC_RECON, index);
}
