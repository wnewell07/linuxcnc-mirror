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
#ifndef RTAPI_SLAB_H
#define RTAPI_SLAB_H

#ifdef __KERNEL__

#include <linux/slab.h>
#include <linux/gfp.h>

#define rtapi_alloc kmalloc
#define rtapi_zalloc kzalloc
#define rtapi_realloc krealloc
#define rtapi_free kfree
#define rtapi_gfp_t gfp_t

enum rtapi_gfp_e {
    RTAPI_GFP_KERNEL = GFP_KERNEL,
    RTAPI_GFP_USER = GFP_USER,
    RTAPI_GFP_DMA = GFP_DMA,
};

#else

#include <stdlib.h>         // malloc/free

enum rtapi_gfp_e {
    RTAPI_GFP_BUFFER,
    RTAPI_GFP_ATOMIC,
    RTAPI_GFP_KERNEL,
    RTAPI_GFP_USER,
    RTAPI_GFP_NOBUFFER,
    RTAPI_GFP_NFS,
    RTAPI_GFP_DMA
};

typedef unsigned long rtapi_gfp_t;

static inline void *rtapi_alloc(size_t size, rtapi_gfp_t flags)
{
    return malloc(size);
}

static inline void *rtapi_zalloc(size_t sz, rtapi_gfp_t flags)
{
    return calloc(sz, 1);
}

static inline void *rtapi_realloc(void *ptr, size_t size, rtapi_gfp_t flags)
{
    return realloc(ptr, size);
}

static inline void rtapi_free(void *obj)
{
    free(obj);
}

#endif

#endif
