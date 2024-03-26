obj-$(CONFIG_SYNA_ISP)	:=	isp.o

ISP_ABS_PATH := $(srctree)/drivers/soc/berlin/modules/isp

ifeq ($(CONFIG_ISP_DOLPHIN),y)
ISP_INC_PATH += -I$(ISP_ABS_PATH)/dolphin
else ifeq ($(CONFIG_ISP_DOLPHIN_A0),y)
ISP_INC_PATH += -I$(ISP_ABS_PATH)/dolphin_a0
endif
isp-objs +=  dhub/configs/dolphin/isp_dhub_cfg.o
ISP_INC_PATH += -I$(ISP_ABS_PATH)/fw_inc/dolphin

HEADER_PATH +=  $(ISP_INC_PATH)    \
		-I$(srctree)/drivers/soc/berlin/modules/isp/dhub \
		-I$(srctree)/drivers/soc/berlin/modules/msgQ \

ccflags-y += $(HEADER_PATH)

isp-objs += 	isp_driver.o \
		drv_isp.o \
		dhub/drv_ispdhub.o \
		dhub/ispdhub_sema.o
