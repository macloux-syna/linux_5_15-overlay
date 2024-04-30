#!/bin/sh
rmmod ./i2c/vvcam_i2c.ko
rmmod ./mipi/vvcam_mipi.ko
rmmod ./isp/vvcam_isp.ko
rmmod ./vb/vvcam_vb.ko
rmmod ./mi_subsystem/vvcam_mss.ko

if [ -n "$1" ];then
#remove video in driver.
    if [ "$1" = "vi" ]; then
    rmmod ./videoin/vvcam_vi.ko
    fi
fi
