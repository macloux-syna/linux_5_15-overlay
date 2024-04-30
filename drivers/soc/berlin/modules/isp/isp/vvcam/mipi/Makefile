ifeq ($(VVCAM_PATH),)
	VVCAM_PATH:=$(shell cd $(shell pwd)/../; pwd)
	export VVCAM_PATH
endif

include $(PWD)/../Param.mk
TARGET = vvcam_mipi

obj-m +=$(TARGET).o
$(TARGET)-objs += vvcam_mipi_platform.o
$(TARGET)-objs += vvcam_mipi_driver.o
ifeq ($(MIPI_VERSION), NWL)
$(TARGET)-objs += vvcam_mipi_hal.o
else
$(TARGET)-objs += vvcam_mipi_dwc_hal.o
endif

PWD :=$(shell pwd)

all:
	make -C $(KERNEL) M=$(PWD) modules
modules_install:
	make -C $(KERNEL) M=$(SRC) modules_install
clean:
	rm -rf $($(TARGET)-objs)
	make -C $(KERNEL) M=$(PWD) clean

