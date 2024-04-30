ifeq ($(VVCAM_PATH),)
	VVCAM_PATH:=$(shell cd $(shell pwd)/../; pwd)
	export VVCAM_PATH
endif

include $(PWD)/../Param.mk
TARGET = vvcam_dw200

obj-m +=$(TARGET).o
$(TARGET)-objs += vvcam_dw_platform.o
$(TARGET)-objs += vvcam_dw_driver.o
$(TARGET)-objs += vvcam_dwe_hal.o
$(TARGET)-objs += vvcam_vse_hal.o
$(TARGET)-objs += ../common/vvcam_event.o
EXTRA_CFLAGS += -I$(PWD)/../common/

PWD :=$(shell pwd)

all:
	make -C $(KERNEL) M=$(PWD) modules
modules_install:
	make -C $(KERNEL) M=$(SRC) modules_install
clean:
	rm -rf $($(TARGET)-objs)
	make -C $(KERNEL) M=$(PWD) clean

