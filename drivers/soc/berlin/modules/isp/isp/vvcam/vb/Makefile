ifeq ($(VVCAM_PATH),)
	VVCAM_PATH:=$(shell cd $(shell pwd)/../; pwd)
	export VVCAM_PATH
endif

include $(PWD)/../Param.mk
TARGET = vvcam_vb

obj-m +=$(TARGET).o
$(TARGET)-objs += vvcam_vb_platform.o
$(TARGET)-objs += vvcam_vb_driver.o
$(TARGET)-objs += vvcam_mmz.o
$(TARGET)-objs += vvcam_cma.o
$(TARGET)-objs += vvcam_vb_procfs.o
PWD :=$(shell pwd)

all:
	make -C $(KERNEL) M=$(PWD) modules
modules_install:
	make -C $(KERNEL) M=$(SRC) modules_install
clean:
	rm -rf $($(TARGET)-objs)
	make -C $(KERNEL) M=$(PWD) clean