#!/bin/sh
rmmod ./i2c/vvcam_i2c.ko
rmmod ./mipi/vvcam_mipi.ko
rmmod ./isp/vvcam_isp.ko
rmmod ./vb/vvcam_vb.ko
rmmod ./v4l2/video/vvcam_video.ko
rmmod ./v4l2/isp/vvcam_isp_subdev.ko
