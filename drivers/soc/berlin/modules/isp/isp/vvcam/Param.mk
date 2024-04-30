# KERNEL :=/lib/modules/$(shell uname -r)/build
ISP_VERSION := ISP8000L_V2002
DW_VERSION := DW200_V40
MIPI_VERSION := NWL
DOLPHIN=1
VVCAM_PATH := $(srctree)/drivers/soc/berlin/modules/isp/isp/vvcam/
PLATFORM_DEVICE := 0

ifeq ($(ISP_VERSION), ISP8000_V2107)
include $(VVCAM_PATH)/isp_version/ISP8000_V2107.mk
endif

ifeq ($(ISP_VERSION), ISP8000_V2201)
include $(VVCAM_PATH)/isp_version/ISP8000_V2201.mk
endif

ifeq ($(ISP_VERSION), ISP8000_V2203)
include $(VVCAM_PATH)/isp_version/ISP8000_V2203.mk
endif

ifeq ($(ISP_VERSION), ISP8200_V2204)
include $(VVCAM_PATH)/isp_version/ISP8200_V2204.mk
endif

ifeq ($(ISP_VERSION), ISP8200L_V2304)
include $(VVCAM_PATH)/isp_version/ISP8200L_V2304.mk
endif

ifeq ($(ISP_VERSION), ISP8210L_V2302)
include $(VVCAM_PATH)/isp_version/ISP8210L_V2302.mk
endif

ifeq ($(ISP_VERSION), ISP8000L_V2305)
include $(VVCAM_PATH)/isp_version/ISP8000L_V2305.mk
endif

ifeq ($(ISP_VERSION), ISP8000L_V2306)
include $(VVCAM_PATH)/isp_version/ISP8000L_V2306.mk
endif

ifeq ($(ISP_VERSION), ISP8000_V2011)
include $(VVCAM_PATH)/isp_version/ISP8000_V2011.mk
endif

ifeq ($(ISP_VERSION), ISP8000L_V2002)
include $(VVCAM_PATH)/isp_version/ISP8000L_V2002.mk
endif

ifeq ($(ISP_VERSION), ISP8000L_V2103)
include $(VVCAM_PATH)/isp_version/ISP8000L_V2103.mk
endif

ifeq ($(ISP_VERSION), ISP8000_V2104)
include $(VVCAM_PATH)/isp_version/ISP8000_V2104.mk
endif

ifeq ($(ISP_VERSION), ISP8200_V2303)
include $(VVCAM_PATH)/isp_version/ISP8200_V2303.mk
endif

ifeq ($(ISP_VERSION), ISP8200_V2307)
include $(VVCAM_PATH)/isp_version/ISP8200_V2307.mk
endif

ifeq ($(ISP_VERSION), ISP8000_V2308)
include $(VVCAM_PATH)/isp_version/ISP8000_V2308.mk
endif

ifeq ($(ISP_VERSION), ISP8200_V2311)
include $(VVCAM_PATH)/isp_version/ISP8200_V2311.mk
endif

EXTRA_CFLAGS += -DVVCAM_I2C_SIZE=0x4
EXTRA_CFLAGS += -DVVCAM_I2C_BUS0=8
EXTRA_CFLAGS += -DVVCAM_I2C_BUS1=9
EXTRA_CFLAGS += -DVVCAM_I2C_BUS2=10
EXTRA_CFLAGS += -DVVCAM_I2C_BUS3=11


ifeq ($(ISP_VI200), 1)
MIPI_VERSION := DWC

EXTRA_CFLAGS += -DVVCAM_TPG0=0xDD308300
EXTRA_CFLAGS += -DVVCAM_TPG1=0xDD308310
EXTRA_CFLAGS += -DVVCAM_I2C0_BASE=0xDD3080f0
EXTRA_CFLAGS += -DVVCAM_I2C1_BASE=0xDD3080f4
EXTRA_CFLAGS += -DVVCAM_I2C2_BASE=0xDD3080f8
EXTRA_CFLAGS += -DVVCAM_I2C3_BASE=0xDD3080fc

EXTRA_CFLAGS += -DVVCAM_CSI0_BASE=0xDD200000
EXTRA_CFLAGS += -DVVCAM_CSI1_BASE=0xDD200800
EXTRA_CFLAGS += -DVVCAM_CSI2_BASE=0xDD201000
EXTRA_CFLAGS += -DVVCAM_CSI3_BASE=0xDD201800
EXTRA_CFLAGS += -DVVCAM_CSI_REG_SIZE=0x800

EXTRA_CFLAGS += -DVVCAM_CSI0_CTRL=0xDE308240
EXTRA_CFLAGS += -DVVCAM_CSI1_CTRL=0xDD308244
EXTRA_CFLAGS += -DVVCAM_CSI2_CTRL=0xDE308248
EXTRA_CFLAGS += -DVVCAM_CSI3_CTRL=0xDE30824C
EXTRA_CFLAGS += -DVVCAM_CSI_CTRL_SIZE=0x4

EXTRA_CFLAGS += -DVVCAM_CSI0_MUX_CTRL=0xDD208304
EXTRA_CFLAGS += -DVVCAM_CSI1_MUX_CTRL=0xDD308304
EXTRA_CFLAGS += -DVVCAM_CSI2_MUX_CTRL=0xDD20C304
EXTRA_CFLAGS += -DVVCAM_CSI3_MUX_CTRL=0xDD30C304
EXTRA_CFLAGS += -DVVCAM_CSI_MUX_CTRL_SIZE=0x4

EXTRA_CFLAGS += -DVVCAM_ISP_CTRL=0xDE308270
EXTRA_CFLAGS += -DVVCAM_ISP_CTRL_SIZE=0x4

else
EXTRA_CFLAGS += -DVVCAM_TPG0=0xDE308300
EXTRA_CFLAGS += -DVVCAM_TPG1=0xDE308310
EXTRA_CFLAGS += -DVVCAM_I2C0_BASE=0xDE3080f0
EXTRA_CFLAGS += -DVVCAM_I2C1_BASE=0xDE3080f4
EXTRA_CFLAGS += -DVVCAM_I2C2_BASE=0xDE3080f8
EXTRA_CFLAGS += -DVVCAM_I2C3_BASE=0xDE3080fc

EXTRA_CFLAGS += -DVVCAM_CSI0_BASE=0xDE200000
EXTRA_CFLAGS += -DVVCAM_CSI1_BASE=0xDE300000
EXTRA_CFLAGS += -DVVCAM_CSI2_BASE=0xDE204000
EXTRA_CFLAGS += -DVVCAM_CSI3_BASE=0xDE304000
EXTRA_CFLAGS += -DVVCAM_CSI_REG_SIZE=0x4000

EXTRA_CFLAGS += -DVVCAM_CSI0_CTRL=0xDE308240
EXTRA_CFLAGS += -DVVCAM_CSI1_CTRL=0xDE308244
EXTRA_CFLAGS += -DVVCAM_CSI2_CTRL=0xDE308248
EXTRA_CFLAGS += -DVVCAM_CSI3_CTRL=0xDE30824C
EXTRA_CFLAGS += -DVVCAM_CSI_CTRL_SIZE=0x4

EXTRA_CFLAGS += -DVVCAM_CSI0_MUX_CTRL=0xDE208304
EXTRA_CFLAGS += -DVVCAM_CSI1_MUX_CTRL=0xDE308304
EXTRA_CFLAGS += -DVVCAM_CSI2_MUX_CTRL=0xDE20C304
EXTRA_CFLAGS += -DVVCAM_CSI3_MUX_CTRL=0xDE30C304
EXTRA_CFLAGS += -DVVCAM_CSI_MUX_CTRL_SIZE=0x4

EXTRA_CFLAGS += -DVVCAM_ISP_CTRL=0xDE308270
EXTRA_CFLAGS += -DVVCAM_ISP_CTRL_SIZE=0x4

endif

ifeq ($(ISP_VERSION), ISP8000L_V2103)
EXTRA_CFLAGS += -DINPUT_CSI_MUX_8000L
endif

EXTRA_CFLAGS += -DVVCAM_TPG2=0xDE308320
EXTRA_CFLAGS += -DVVCAM_TPG3=0xDE308330
EXTRA_CFLAGS += -DVVCAM_TPG_SIZE=0x4

ifeq ($(ISP_VERSION), ISP8000NANO_V2012)
EXTRA_CFLAGS += -DVVCAM_CSI_BUS_WIDTH=12
else
EXTRA_CFLAGS += -DVVCAM_CSI_BUS_WIDTH=16
endif

ifeq ($(ISP_VERSION), $(findstring $(ISP_VERSION), ISP8000_V2108 ISP8000_V2202 ISP8000_V2203 ISP8200L_V2304 ISP8210L_V2302 ISP8000L_V2305 ISP8200_V2303 ISP8200_V2307 ISP8000_V2308 ISP8200_V2311 ISP8000L_V2310 ISP8000L_V2002))
EXTRA_CFLAGS += -DCSI_CTRL_MUX_T0T1=1
EXTRA_CFLAGS += -DCSI_CTRL_MUX_T2T3=0
else
EXTRA_CFLAGS += -DCSI_CTRL_MUX_T0T1=0
EXTRA_CFLAGS += -DCSI_CTRL_MUX_T2T3=0
endif


ifeq ($(ISP_VERSION), ISP8000_V2009)
EXTRA_CFLAGS += -DVVCAM_ISP0_BASE=0xDE600000
EXTRA_CFLAGS += -DVVCAM_ISP1_BASE=0xDE700000

else ifeq ($(ISP_VI200), 1)
EXTRA_CFLAGS += -DVVCAM_ISP0_BASE=0xDD000000
EXTRA_CFLAGS += -DVVCAM_ISP1_BASE=0xDE800000
EXTRA_CFLAGS += -DVVCAM_VI_BASE=0xDD100000
EXTRA_CFLAGS += -DVVCAM_VI_REG_SIZE=0x10000
EXTRA_CFLAGS += -DVVCAM_VI_IRQ=16
else
EXTRA_CFLAGS += -DVVCAM_ISP0_BASE=0xF9150000
EXTRA_CFLAGS += -DVVCAM_ISP1_BASE=0xDE100000
endif

EXTRA_CFLAGS += -DVVCAM_ISP_REG_SIZE=0x10000

EXTRA_CFLAGS += -DVVCAM_ISP0_IRQ=16
EXTRA_CFLAGS += -DVVCAM_ISP0_MI_IRQ=16
EXTRA_CFLAGS += -DVVCAM_ISP0_FE_IRQ=16
EXTRA_CFLAGS += -DVVCAM_ISP0_FUSA_IRQ=16

EXTRA_CFLAGS += -DVVCAM_ISP1_IRQ=16
EXTRA_CFLAGS += -DVVCAM_ISP1_MI_IRQ=16
EXTRA_CFLAGS += -DVVCAM_ISP1_FE_IRQ=16
EXTRA_CFLAGS += -DVVCAM_ISP1_FUSA_IRQ=16

EXTRA_CFLAGS += -DVVCAM_RMEM_BASE=0x10000000
EXTRA_CFLAGS += -DVVCAM_RMEM_SIZE=0x20000000


EXTRA_CFLAGS += -DVVCAM_MSS_RESET_BASE=0xDE308260
EXTRA_CFLAGS += -DVVCAM_MSS_RESET_SIZE=0x4
EXTRA_CFLAGS += -DVVCAM_MSS_BASE=0xDE500000
EXTRA_CFLAGS += -DVVCAM_MSS_REG_SIZE=0x10000


ifeq ($(DW_VERSION), $(findstring $(DW_VERSION), DW200_V40 DW200_V41))
EXTRA_CFLAGS += -DVVCAM_DWE_REG_BASE=0xDE381C00
EXTRA_CFLAGS += -DVVCAM_DWE_REG_SIZE=0x00000800
EXTRA_CFLAGS += -DVVCAM_VSE_REG_BASE=0xDE380000
EXTRA_CFLAGS += -DVVCAM_VSE_REG_SIZE=0x00001C00
endif

ifeq ($(DW_VERSION), DW200_V42)
EXTRA_CFLAGS += -DDW200_V42
EXTRA_CFLAGS += -DVVCAM_DWE_REG_BASE=0xDE382C00
EXTRA_CFLAGS += -DVVCAM_DWE_REG_SIZE=0x00000800
EXTRA_CFLAGS += -DVVCAM_VSE_REG_BASE=0xDE380000
EXTRA_CFLAGS += -DVVCAM_VSE_REG_SIZE=0x00002C00
endif

ifeq ($(DW_VERSION), DW200_V30)
EXTRA_CFLAGS += -DVVCAM_DWE_REG_BASE=0xDE380C00
EXTRA_CFLAGS += -DVVCAM_DWE_REG_SIZE=0x00000800
EXTRA_CFLAGS += -DVVCAM_VSE_REG_BASE=0xDE380000
EXTRA_CFLAGS += -DVVCAM_VSE_REG_SIZE=0x00000C00
endif

EXTRA_CFLAGS += -DVVCAM_DW_RST_REG_BASE=0xDE308250
EXTRA_CFLAGS += -DVVCAM_DW_RST_REG_SIZE=0x4

EXTRA_CFLAGS += -DVVCAM_DWE_IRQ=16
EXTRA_CFLAGS += -DVVCAM_VSE_IRQ=16
EXTRA_CFLAGS += -DVVCAM_DW_FE_IRQ=16

EXTRA_CFLAGS += -O2 -Werror

ifeq ($(PLATFORM_DEVICE), 1)
EXTRA_CFLAGS += -DVVCAM_PLATFORM_REGISTER
endif

PLATFORM_SUBDEVICE := 0

ifeq ($(PLATFORM_SUBDEVICE), 1)
EXTRA_CFLAGS += -DVVCAM_SUBDEV_PLATFORM_REGISTER
endif

EXTRA_CFLAGS += -DVVCAM_ISP_DEV_MAX=2

ifeq ($(DOLPHIN), 1)
EXTRA_CFLAGS += -DDOLPHIN
endif