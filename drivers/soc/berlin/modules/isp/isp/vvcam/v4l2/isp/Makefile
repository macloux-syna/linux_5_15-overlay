ifeq ($(VVCAM_PATH),)
	VVCAM_PATH:=$(shell cd $(shell pwd)/../../; pwd)
	export VVCAM_PATH
endif

include $(PWD)/../../Param.mk
TARGET = vvcam_isp_subdev

obj-m +=$(TARGET).o
$(TARGET)-objs += vvcam_isp_platform.o
$(TARGET)-objs += vvcam_isp_driver.o
$(TARGET)-objs += vvcam_isp_event.o
$(TARGET)-objs += vvcam_isp_procfs.o
$(TARGET)-objs += isp_ctrl/vvcam_isp_ctrl.o

EXTRA_CFLAGS += -I$(PWD)
EXTRA_CFLAGS += -I$(PWD)/../common/
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl


ifeq ($(ISP_AE_V3), 1)
$(TARGET)-objs += isp_ctrl/ae/ae_v3/vvcam_isp_ae.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/ae/ae_v3/
endif

ifeq ($(ISP_AWB_V3), 1)
$(TARGET)-objs += isp_ctrl/awb/awb_v3/vvcam_isp_awb.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/awb/awb_v3/
endif

ifeq ($(ISP_GC_V1), 1)
$(TARGET)-objs += isp_ctrl/gc/gc_v1/vvcam_isp_gc.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/gc/gc_v1/
endif

ifeq ($(ISP_GC_V2), 1)
$(TARGET)-objs += isp_ctrl/gc/gc_v2/vvcam_isp_gc.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/gc/gc_v2/
endif

ifeq ($(ISP_2DNR_V3), 1)
$(TARGET)-objs += isp_ctrl/2dnr/2dnr_v3/vvcam_isp_2dnr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/2dnr/2dnr_v3/
endif

ifeq ($(ISP_2DNR_V5_2), 1)
$(TARGET)-objs += isp_ctrl/2dnr/2dnr_v5_2/vvcam_isp_2dnr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/2dnr/2dnr_v5_2/
endif

ifeq ($(ISP_EE_V1), 1)
$(TARGET)-objs += isp_ctrl/ee/ee_v1/vvcam_isp_ee.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/ee/ee_v1/
endif

ifeq ($(ISP_EE_V2), 1)
$(TARGET)-objs += isp_ctrl/ee/ee_v2/vvcam_isp_ee.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/ee/ee_v2/
endif

ifeq ($(ISP_CPROC), 1)
$(TARGET)-objs += isp_ctrl/cproc/cproc/vvcam_isp_cproc.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/cproc/cproc/
endif

ifeq ($(ISP_WDR_V3), 1)
$(TARGET)-objs += isp_ctrl/wdr/wdr_v3/vvcam_isp_wdr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/wdr/wdr_v3/
endif

ifeq ($(ISP_WDR_V5_2), 1)
$(TARGET)-objs += isp_ctrl/wdr/wdr_v5_2/vvcam_isp_wdr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/wdr/wdr_v5_2/
endif

ifeq ($(ISP_BLS), 1)
$(TARGET)-objs += isp_ctrl/bls/bls/vvcam_isp_bls.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/bls/bls/
endif

ifeq ($(ISP_CCM), 1)
$(TARGET)-objs += isp_ctrl/ccm/ccm/vvcam_isp_ccm.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/ccm/ccm/
endif

ifeq ($(ISP_DPCC), 1)
$(TARGET)-objs += isp_ctrl/dpcc/dpcc/vvcam_isp_dpcc.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/dpcc/dpcc/
endif

ifeq ($(ISP_GE), 1)
$(TARGET)-objs += isp_ctrl/ge/ge/vvcam_isp_ge.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/ge/ge/
endif

ifeq ($(ISP_LSC_V1), 1)
$(TARGET)-objs += isp_ctrl/lsc/lsc_v1/vvcam_isp_lsc.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/lsc/lsc_v1/
endif

ifeq ($(ISP_LSC_V3), 1)
$(TARGET)-objs += isp_ctrl/lsc/lsc_v3/vvcam_isp_lsc.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/lsc/lsc_v3/
endif

ifeq ($(ISP_WB), 1)
$(TARGET)-objs += isp_ctrl/wb/wb/vvcam_isp_wb.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/wb/wb/
endif

ifeq ($(ISP_3DNR_V1_2), 1)
$(TARGET)-objs += isp_ctrl/3dnr/3dnr_v1_2/vvcam_isp_3dnr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/3dnr/3dnr_v1_2
endif

ifeq ($(ISP_3DNR_V2_1), 1)
$(TARGET)-objs += isp_ctrl/3dnr/3dnr_v2_1/vvcam_isp_3dnr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/3dnr/3dnr_v2_1
endif

ifeq ($(ISP_AF_V3), 1)
$(TARGET)-objs += isp_ctrl/af/af_v3/vvcam_isp_af.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/af/af_v3
endif

ifeq ($(ISP_AFM_V1), 1)
$(TARGET)-objs += isp_ctrl/afm/afm_v1/vvcam_isp_afm.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/afm/afm_v1/
endif

ifeq ($(ISP_DMSC_V1), 1)
$(TARGET)-objs += isp_ctrl/dmsc/dmsc_v1/vvcam_isp_dmsc.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/dmsc/dmsc_v1/
endif

ifeq ($(ISP_DMSC_V2), 1)
$(TARGET)-objs += isp_ctrl/dmsc/dmsc_v2/vvcam_isp_dmsc.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/dmsc/dmsc_v2/
endif

ifeq ($(ISP_EXP_V2), 1)
$(TARGET)-objs += isp_ctrl/exp/exp_v2/vvcam_isp_exp.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/exp/exp_v2/
endif

ifeq ($(ISP_GTM_V1), 1)
$(TARGET)-objs += isp_ctrl/gtm/gtm_v1/vvcam_isp_gtm.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/gtm/gtm_v1
endif

ifeq ($(ISP_HDR_V1_3), 1)
$(TARGET)-objs += isp_ctrl/hdr/hdr_v1_3/vvcam_isp_hdr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/hdr/hdr_v1_3
endif

ifeq ($(ISP_HDR_V3_1), 1)
$(TARGET)-objs += isp_ctrl/hdr/hdr_v3_1/vvcam_isp_hdr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/hdr/hdr_v3_1
endif

ifeq ($(ISP_RGBIR_V2), 1)
$(TARGET)-objs += isp_ctrl/rgbir/rgbir_v2/vvcam_isp_rgbir.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/rgbir/rgbir_v2
endif

ifeq ($(ISP_DG), 1)
$(TARGET)-objs += isp_ctrl/dg/dg/vvcam_isp_dg.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/dg/dg
endif

ifeq ($(ISP_CPD), 1)
$(TARGET)-objs += isp_ctrl/cpd/cpd/vvcam_isp_cpd.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/cpd/cpd
endif

ifeq ($(ISP_DPF_V1), 1)
$(TARGET)-objs += isp_ctrl/dpf/dpf_v1/vvcam_isp_dpf.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/dpf/dpf_v1
endif

ifeq ($(ISP_CNR_V1), 1)
$(TARGET)-objs += isp_ctrl/cnr/cnr_v1/vvcam_isp_cnr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/cnr/cnr_v1
endif

ifeq ($(ISP_GWDR_V1), 1)
$(TARGET)-objs += isp_ctrl/gwdr/gwdr_v1/vvcam_isp_gwdr.o
EXTRA_CFLAGS += -I$(PWD)/isp_ctrl/gwdr/gwdr_v1
endif

PWD :=$(shell pwd)
SYM := /usr/src/linux-headers-$(shell uname -r)/Module.symvers

all:
	make -C $(KERNEL) M=$(PWD) modules
modules_install:
	make -C $(KERNEL) M=$(SRC) modules_install
clean:
	rm -rf $($(TARGET)-objs)
	make -C $(KERNEL) M=$(PWD) clean

