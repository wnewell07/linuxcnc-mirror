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
#ifndef RTAPI_DEVICE_H
#define RTAPI_DEVICE_H

#ifdef __KERNEL__

#include <linux/device.h>
#include <linux/version.h>
#define rtapi_device device
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
static inline void rtapi_device_set_name(struct rtapi_device *dev, const char *name)
{
    strncpy(dev->bus_id, name, sizeof(dev->bus_id));
    dev->bus_id[sizeof(dev->bus_id) - 1] = '\0';
}
#else
#define rtapi_device_set_name dev_set_name
#endif
#define rtapi_device_register device_register
#define rtapi_device_unregister device_unregister

#else

#include <rtapi.h>

struct rtapi_device {
    char    name[RTAPI_NAME_LEN+1];
    void    (*release)(struct rtapi_device *dev);
};

extern int rtapi_device_set_name(struct rtapi_device *dev, const char *name);

extern int rtapi_device_register(struct rtapi_device *dev);

extern void rtapi_device_unregister(struct rtapi_device *dev);


#endif

#endif
