/****************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 VeriSilicon Holdings Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************
 *
 * The GPL License (GPL)
 *
 * Copyright (c) 2023 VeriSilicon Holdings Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 *****************************************************************************
 *
 * Note: This software is released under dual MIT and GPL licenses. A
 * recipient may use this file under the terms of either the MIT license or
 * GPL License. If you wish to use only one license not the other, you can
 * indicate your decision by deleting one of the above license notices in your
 * version of this file.
 *
 *****************************************************************************/

#include <linux/module.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>

#include "soc_i2c_platform.h"

static int devices_mask = 0x01;
MODULE_PARM_DESC(devices_mask, "I2C devices mask");

static void soc_gpio_i2c_release(struct device *dev)
{
    dev_info(dev, "%s\n", __func__);
	return;
}

static struct resource soc_gpio_i2c0_resource[] = {
    [0] = {
        .start = VVCAM_I2C0_BASE,
        .end = VVCAM_I2C0_BASE + VVCAM_I2C_SIZE - 1,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device soc_gpio_i2c0_pdev = {
	.name = "soc_gpio_i2c",
	.id = VVCAM_I2C_BUS0,
	.resource = soc_gpio_i2c0_resource,
	.num_resources = ARRAY_SIZE(soc_gpio_i2c0_resource),
	.dev.release = soc_gpio_i2c_release,
};

static struct resource soc_gpio_i2c1_resource[] = {
    [0] = {
        .start = VVCAM_I2C1_BASE,
        .end = VVCAM_I2C1_BASE + VVCAM_I2C_SIZE - 1,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device soc_gpio_i2c1_pdev = {
	.name = "soc_gpio_i2c",
	.id = VVCAM_I2C_BUS1,
	.resource = soc_gpio_i2c1_resource,
	.num_resources = ARRAY_SIZE(soc_gpio_i2c1_resource),
	.dev.release = soc_gpio_i2c_release,
};

static struct resource soc_gpio_i2c2_resource[] = {
    [0] = {
        .start = VVCAM_I2C2_BASE,
        .end = VVCAM_I2C2_BASE + VVCAM_I2C_SIZE - 1,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device soc_gpio_i2c2_pdev = {
	.name = "soc_gpio_i2c",
	.id = VVCAM_I2C_BUS2,
	.resource = soc_gpio_i2c2_resource,
	.num_resources = ARRAY_SIZE(soc_gpio_i2c2_resource),
	.dev.release = soc_gpio_i2c_release,
};

static struct resource soc_gpio_i2c3_resource[] = {
    [0] = {
        .start = VVCAM_I2C3_BASE,
        .end = VVCAM_I2C3_BASE + VVCAM_I2C_SIZE - 1,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device soc_gpio_i2c3_pdev = {
	.name = "soc_gpio_i2c",
	.id = VVCAM_I2C_BUS3,
	.resource = soc_gpio_i2c3_resource,
	.num_resources = ARRAY_SIZE(soc_gpio_i2c3_resource),
	.dev.release = soc_gpio_i2c_release,
};

int soc_gpio_i2c_platform_device_register(void)
{
    int ret = 0;
    int devs_mask = 0;
    if (devices_mask & 0x01) {
        ret = platform_device_register(&soc_gpio_i2c0_pdev);
        if (ret) {
            goto err_register;
        }
        devs_mask |= 0x01;
    }

    if (devices_mask & 0x02) {
        ret = platform_device_register(&soc_gpio_i2c1_pdev);
        if (ret) {
            goto err_register;
        }
        devs_mask |= 0x02;
    }

    if (devices_mask & 0x04) {
        ret = platform_device_register(&soc_gpio_i2c2_pdev);
        if (ret) {
            goto err_register;
        }
        devs_mask |= 0x04;
    }

    if (devices_mask & 0x08) {
        ret = platform_device_register(&soc_gpio_i2c3_pdev);
        if (ret) {
            goto err_register;
        }
        devs_mask |= 0x04;
    }

    return ret;

err_register:
    if (devs_mask & 0x01)
		platform_device_unregister(&soc_gpio_i2c0_pdev);
	if (devs_mask & 0x02)
		platform_device_unregister(&soc_gpio_i2c1_pdev);
	if (devs_mask & 0x04)
		platform_device_unregister(&soc_gpio_i2c2_pdev);
	if (devs_mask & 0x08)
		platform_device_unregister(&soc_gpio_i2c3_pdev);

    return ret;
}

void soc_gpio_i2c_platform_device_unregister(void)
{
    if (devices_mask & 0x01)
		platform_device_unregister(&soc_gpio_i2c0_pdev);
	if (devices_mask & 0x02)
		platform_device_unregister(&soc_gpio_i2c1_pdev);
	if (devices_mask & 0x04)
		platform_device_unregister(&soc_gpio_i2c2_pdev);
	if (devices_mask & 0x08)
		platform_device_unregister(&soc_gpio_i2c3_pdev);

    return;
}

module_param(devices_mask, int, 0644);
