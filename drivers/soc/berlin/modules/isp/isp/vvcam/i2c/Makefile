ifeq ($(VVCAM_PATH),)
	VVCAM_PATH:=$(shell cd $(shell pwd)/../; pwd)
	export VVCAM_PATH
endif

include $(PWD)/../Param.mk
TARGET = vvcam_i2c

obj-m +=$(TARGET).o
$(TARGET)-objs += soc_i2c_platform.o
$(TARGET)-objs += soc_i2c_driver.o
$(TARGET)-objs += soc_i2c_procfs.o

PWD :=$(shell pwd)

all:
	make -C $(KERNEL) M=$(PWD) modules
modules_install:
	make -C $(KERNEL) M=$(SRC) modules_install
clean:
	rm -rf $($(TARGET)-objs)
	make -C $(KERNEL) M=$(PWD) clean

