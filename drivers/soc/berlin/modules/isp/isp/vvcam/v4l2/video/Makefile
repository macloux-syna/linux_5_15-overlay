ifeq ($(VVCAM_PATH),)
	VVCAM_PATH:=$(shell cd $(shell pwd)/../../; pwd)
	export VVCAM_PATH
endif

include $(PWD)/../../Param.mk
TARGET = vvcam_video

obj-m +=$(TARGET).o
$(TARGET)-objs += vvcam_video_driver.o
$(TARGET)-objs += vvcam_video_platform.o
$(TARGET)-objs += vvcam_video_register.o
$(TARGET)-objs += vvcam_video_m2m_register.o
$(TARGET)-objs += vvcam_video_event.o
EXTRA_CFLAGS += -I$(PWD)/../common/

PWD :=$(shell pwd)

all:
	make -C $(KERNEL) M=$(PWD) modules KBUILD_EXTRA_SYMBOLS=$(PWD)/../isp/Module.symvers
modules_install:
	make -C $(KERNEL) M=$(SRC) modules_install  KBUILD_EXTRA_SYMBOLS=$(PWD)/../isp/Module.symvers
clean:
	rm -rf $($(TARGET)-objs)
	make -C $(KERNEL) M=$(PWD) clean