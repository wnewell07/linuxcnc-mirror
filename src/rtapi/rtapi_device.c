#ifndef __KERNEL__
/*
 *    Copyright (C) 2013 LinuxCNC Authors
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <rtapi.h>
#include <rtapi_device.h>

extern int rtapi_device_set_name(struct rtapi_device *dev, const char *name) {
    strncpy(dev->name, name, HAL_NAME_LEN);
    dev->name[HAL_NAME_LEN - 1] = '\0';
    return 0;
}

extern int rtapi_device_register(struct rtapi_device *dev) {
    return 0;
}

extern void rtapi_device_unregister(struct rtapi_device *dev) {
    return;
}

#endif
