obj-$(CONFIG_SYNA_ISP) += isp.o
obj-$(CONFIG_SYNA_ISP_SCALER) += scaler.o

ISP_ABS_PATH := $(srctree)/drivers/soc/berlin/modules/isp

ifeq ($(CONFIG_ISP_DOLPHIN),y)
 ccflags-y += -D DOLPHIN
endif

ifeq ($(CONFIG_SYNA_ISP_SCALER), m)
ccflags-y += -DSYNA_ISP_BE_A0
endif

HEADER_PATH +=  $(ISP_INC_PATH)    \
		-I$(srctree)/drivers/soc/berlin/modules/isp \
		-I$(srctree)/drivers/soc/berlin/modules/isp/isp \
		-I$(srctree)/drivers/soc/berlin/modules/isp/isp/dhub \
		-I$(srctree)/drivers/soc/berlin/modules/isp/fw \
		-I$(srctree)/drivers/soc/berlin/modules/msgQ

ccflags-y += $(HEADER_PATH)

isp-objs += isp/isp_driver.o \
		isp/drv_isp.o \
		isp/dhub/drv_ispdhub.o \
		isp/dhub/ispdhub_sema.o \
		isp/dhub/configs/dolphin/isp_dhub_cfg.o

HEADER_PATH +=  $(ISP_INC_PATH)    \
		-I$(srctree)/drivers/soc/berlin/modules/isp/ \
		-I$(srctree)/drivers/soc/berlin/modules/isp/dhub \
		-I$(srctree)/drivers/soc/berlin/modules/msgQ \
		-I$(srctree)/drivers/soc/berlin/modules/isp/be/drv/src/scl \
		-I$(srctree)/drivers/soc/berlin/modules/isp/be/drv/src/common \
		-I$(srctree)/drivers/soc/berlin/modules/isp/be/drv/inc \
		-I$(srctree)/drivers/soc/berlin/modules/isp/be/v4l2/memops \
		-I$(srctree)/drivers/soc/berlin/modules/isp/be/v4l2/scaler

ccflags-y += $(HEADER_PATH)

scaler-objs += be/v4l2/scaler/m2m_scaler.o \
		be/v4l2/scaler/scaler_dbg.o \
		be/v4l2/scaler/scaler_drv_api.o \
		be/v4l2/memops/isp_dma_heap.o \
		be/drv/src/scl/ispSS_be_frc_scl.o \
		be/drv/src/scl/ispSS_be_scl_api.o \
		be/drv/src/scl/ispSS_be_scl_apifuncs.o \
		be/drv/src/intf/ispbe_api.o \
		be/drv/src/common/ispSS_fe_client.o \
		be/drv/src/common/ispSS_bcmbuf.o \
		be/drv/src/common/ispSS_bcmbuf_queue.o \
		be/drv/src/common/ispSS_clkrst.o \
		be/drv/src/common/ispSS_api_dhub_config.o \
		be/drv/src/common/ispSS_be_Mtr.o \
		be/drv/src/common/ispSS_dhub_drv.o \
		be/drv/src/common/ispSS_rqstq.o \
		be/drv/src/common/ispSS_shm.o \
		be/drv/src/common/ispSS_reg.o \
		be/drv/src/common/hal_dhub.o
