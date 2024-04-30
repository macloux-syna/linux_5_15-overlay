#!/bin/sh

modprobe i2c-dev
insmod ./i2c/vvcam_i2c.ko devices_mask=0x0f
insmod ./mipi/vvcam_mipi.ko devices_mask=0x0f
insmod ./isp/vvcam_isp.ko
insmod ./vb/vvcam_vb.ko
modprobe videobuf2-v4l2
modprobe videodev
modprobe mc
modprobe v4l2-async
modprobe videobuf2-dma-contig
modprobe v4l2-fwnode
modprobe videobuf2-common
insmod ./v4l2/isp/vvcam_isp_subdev.ko
insmod ./v4l2/video/vvcam_video.ko
