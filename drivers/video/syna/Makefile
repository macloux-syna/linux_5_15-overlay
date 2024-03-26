PWD := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BERLIN_INC := $(srctree)/drivers/soc/berlin/modules/
BERLIN_VPP_INC := $(srctree)/drivers/soc/berlin/modules/vpp/include/
ION_INC := $(srctree)/drivers/staging/android/

###############################################################################
#
# Common FB module objects
#
###############################################################################

obj-$(CONFIG_SYNA_FBDEV) += synafb.o

synafb-objs += source/fb.o

#Enable for double buffering otherwise tripple buffering
MV_DEFINE += -DMRVL_FB_ENABLE_DOUBLE_BUFFERING

MV_DEFINE += -DMV_SYSTEM_WIDTH_DEFAULT=1280
MV_DEFINE += -DMV_SYSTEM_HEIGHT_DEFAULT=720

MV_DEFINE += -I$(BERLIN_INC)/tzd \
             -I$(BERLIN_INC)/tzd/tz/common \
             -I$(BERLIN_INC)/tzd/include


ifneq ($(filter y, $(CONFIG_AVIO_PLATYPUS) $(CONFIG_AVIO_DOLPHIN) $(CONFIG_AVIO_DOLPHIN_A0) $(CONFIG_AVIO_MYNA2)), )
CHIP_INC_DIR_LOCAL= $(BERLIN_INC)/avio/vpp/ca/include/vsxxx
ifeq ($(CONFIG_AVIO_PLATYPUS), y)
FIRMWARE_INC_DIR_LOCAL=  $(BERLIN_INC)/avio/fw_inc/platypus/
else ifeq ($(CONFIG_AVIO_DOLPHIN), y)
FIRMWARE_INC_DIR_LOCAL=  $(BERLIN_INC)/avio/fw_inc/dolphin/
else ifeq ($(CONFIG_AVIO_DOLPHIN_A0), y)
FIRMWARE_INC_DIR_LOCAL=  $(BERLIN_INC)/avio/fw_inc/dolphin_a0/
else ifeq ($(CONFIG_AVIO_MYNA2), y )
FIRMWARE_INC_DIR_LOCAL=  $(BERLIN_INC)/avio/fw_inc/myna2/
CHIP_INC_DIR_LOCAL= $(BERLIN_INC)/avio/vpp/ca/include/asxxx
endif
MV_DEFINE += -I$(CHIP_INC_DIR_LOCAL)
MV_DEFINE += -I$(FIRMWARE_INC_DIR_LOCAL)
endif


MV_DEFINE += -I$(BERLIN_INC)/avio/common
MV_DEFINE += -I$(BERLIN_INC)/avio/vpp/ca/include
MV_DEFINE += -I$(srctree)/drivers/soc/berlin/modules/msgQ

HEADER_PATH  += -I$(BERLIN_VPP_INC)
HEADER_PATH  += -I$(ION_INC)

ccflags-y += $(MV_DEFINE)
ccflags-y += $(HEADER_PATH)
ccflags-y += -Werror
