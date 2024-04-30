#!/bin/sh
# Example: ./insmod.sh server 4sensor 2isp vi mss

param_server="false"
param_sensor_num="2sensor" 
param_isp_num="1isp"
param_videoin="false"
param_mss="false"

if [ -n "$1" ];then
    if [ "$1" = "server" ];then
        param_server="true"
    elif [ "$1" = "4sensor" ];then
        param_sensor_num="4sensor"
    elif [ "$1" = "2isp" ];then
        param_isp_num="2isp"
    elif [ "$1" = "vi" ];then
        param_videoin="true"
    elif [ "$1" = "mss" ];then
        param_mss="true"
    fi
fi

if [ -n "$2" ];then
    if [ "$2" = "server" ];then
        param_server="true"
    elif [ "$2" = "4sensor" ];then
        param_sensor_num="4sensor"
    elif [ "$2" = "2isp" ];then
        param_isp_num="2isp"
    elif [ "$2" = "vi" ];then
        param_videoin="true"
    elif [ "$2" = "mss" ];then
        param_mss="true"
    fi
fi

if [ -n "$3" ];then
    if [ "$3" = "server" ];then
        param_server="true"
    elif [ "$3" = "4sensor" ];then
        param_sensor_num="4sensor"
    elif [ "$3" = "2isp" ];then
        param_isp_num="2isp"
    elif [ "$3" = "vi" ];then
        param_videoin="true"
    elif [ "$3" = "mss" ];then
        param_mss="true"
    fi
fi

if [ -n "$4" ];then
    if [ "$4" = "server" ];then
        param_server="true"
    elif [ "$4" = "4sensor" ];then
        param_sensor_num="4sensor"
    elif [ "$4" = "2isp" ];then
        param_isp_num="2isp"
    elif [ "$4" = "vi" ];then
        param_videoin="true"
    elif [ "$4" = "mss" ];then
        param_mss="true"
    fi
fi

if [ -n "$5" ];then
    if [ "$5" = "server" ];then
        param_server="true"
    elif [ "$5" = "4sensor" ];then
        param_sensor_num="4sensor"
    elif [ "$5" = "2isp" ];then
        param_isp_num="2isp"
    elif [ "$5" = "vi" ];then
        param_videoin="true"
    elif [ "$5" = "mss" ];then
        param_mss="true"
    fi
fi

if [ "$param_server" = "true" ];then
    insmod ./vb/vvcam_vb.ko resmem_cached=1
else
    insmod ./vb/vvcam_vb.ko
fi

if [ "$param_sensor_num" = "4sensor" ];then
    insmod ./i2c/vvcam_i2c.ko devices_mask=0x0f
    insmod ./mipi/vvcam_mipi.ko devices_mask=0x0f
else
    insmod ./i2c/vvcam_i2c.ko devices_mask=0x03
    insmod ./mipi/vvcam_mipi.ko devices_mask=0x03
fi

if [ "$param_isp_num" = "2isp" ];then
    insmod ./isp/vvcam_isp.ko devices_mask=0x03
else
    insmod ./isp/vvcam_isp.ko
fi

if [ "$param_videoin" = "true" ];then
    insmod ./videoin/vvcam_vi.ko
fi

if [ "$param_mss" = "true" ];then
    insmod ./mi_subsystem/vvcam_mss.ko
fi

