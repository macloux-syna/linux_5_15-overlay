###############################################################################
# LCDC - VPP kernel sub-module objects
###############################################################################
AVIO_ABS_PATH := $(srctree)/drivers/soc/berlin/modules/avio
VPP_ABS_PATH := $(srctree)/drivers/soc/berlin/modules/vpp
BCMBUF_ABS_PATH := $(srctree)/drivers/soc/berlin/modules/bcmbuf
LCDC_ABS_PATH := $(AVIO_ABS_PATH)/lcdc

LCDC_INC_PATH := -I$(LCDC_ABS_PATH)/include/
LCDC_INC_PATH += -I$(BCMBUF_ABS_PATH)/include
LCDC_INC_PATH += -I$(BCMBUF_ABS_PATH)/include/configs/chip-as470/

ccflags-y += $(LCDC_INC_PATH)

obj-$(CONFIG_AVIO_MYNA2) += lcdc/source/syna_lcdc_dlr.o
obj-$(CONFIG_AVIO_MYNA2) += lcdc/source/syna_lcdc_dev.o
