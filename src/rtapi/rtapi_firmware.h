/*
 *  Copyright (C) 2013 LinuxCNC Authors
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef RTAPI_FIRMWARE_H
#define RTAPI_FIRMWARE_H

#include <rtapi_device.h>

#ifdef __KERNEL__

#include <linux/firmware.h>
#define rtapi_firmware firmware
#define rtapi_request_firmware request_firmware
#define rtapi_release_firmware release_firmware

#else

#include "rtapi.h"
#include <sys/types.h>
struct rtapi_firmware {
    size_t size;
    const unsigned char *data;
    int fd;
};

extern int rtapi_request_firmware(const struct rtapi_firmware **fw,
    const char *name, struct rtapi_device *device);

extern void rtapi_release_firmware(const struct rtapi_firmware *fw);
#endif

#endif
