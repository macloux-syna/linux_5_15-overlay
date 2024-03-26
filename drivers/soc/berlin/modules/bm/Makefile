obj-$(CONFIG_BERLIN_BM) := amp_bm.o
amp_bm-objs += bm.o

ifeq ($(CONFIG_OPTEE),y)
amp_bm-objs += ptm_wrapper_optee.o
else
INC_PATH = -I$(srctree)/drivers/soc/berlin/modules/tzd/tee/client_api \
           -I$(srctree)/drivers/soc/berlin/modules/tzd/tee/common \
           -I$(srctree)/drivers/soc/berlin/modules/tzd/include \
           -I$(srctree)/drivers/soc/berlin/modules/tzd \
           -I$(srctree)/drivers/soc/berlin/modules/tzd/tz/common
amp_bm-objs += ptm_wrapper_tzd.o
endif

ccflags-y += -Werror
ccflags-y += $(INC_PATH)
