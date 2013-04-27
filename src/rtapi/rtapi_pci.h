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
#ifndef RTAPI_PCI_H
#define RTAPI_PCI_H

#include "config.h"

/***********************************************************************
*                        PCI DEVICE SUPPORT                            *
************************************************************************/
#ifdef __KERNEL__
#include <linux/pci.h>
#include <linux/version.h>
#define rtapi_iomem __iomem
#define rtapi_iounmap iounmap
#define rtapi_pci_device_id pci_device_id
#define rtapi_pci_dev pci_dev
#define rtapi_pci_disable_device pci_disable_device
#define rtapi_pci_driver pci_driver
#define rtapi_pci_enable_device pci_enable_device
#define rtapi_pci_get_device pci_get_device
#define rtapi_pci_ioremap pci_ioremap
#define rtapi_pci_iounmap pci_iounmap
#define rtapi_pci_name pci_name
#define rtapi_pci_put_device pci_put_device
#define rtapi_pci_register_driver pci_register_driver
#define rtapi_pci_resource_len pci_resource_len
#define rtapi_pci_resource_start pci_resource_start
#define rtapi_pci_set_drvdata pci_set_drvdata
#define rtapi_pci_unregister_driver pci_unregister_driver
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
static inline void *__iomem
rtapi_pci_ioremap_bar(struct rtapi_pci_dev *dev, int bar)
{
    return ioremap_nocache(pci_resource_start((struct pci_dev*)dev, bar),
	pci_resource_len((struct pci_dev*)dev, bar));
}
#else
#define rtapi_pci_ioremap_bar pci_ioremap_bar
#endif
#else

#define rtapi_iomem /* NOTHING */

/*---------------------------------------------------------------------**
** Structures and defines that should be pulled in from <linux/pci.h>  **
** ...except we're not compiling kernel code.  :(                      **
**---------------------------------------------------------------------*/

struct rtapi_pci_device_id {
    __u32 vendor, device;           /* Vendor and device ID or PCI_ANY_ID*/
    __u32 subvendor, subdevice;     /* Subsystem ID's or PCI_ANY_ID */
    __u32 class, class_mask;        /* (class,subclass,prog-if) triplet */
};

/** FIXME: Support more thane one board! **/
struct rtapi_pci_resource {
    void            *start;         /* Details read from device/resource file */
    void            *end;
    unsigned long   flags;
    int             fd;             /* sysfs_pci device/resourceN file descriptor */
    void            *mmio;          /* The mmap address (if mapped) */
};

struct rtapi_pci_dev {
    char            dev_name[32];   /* Device name (0000:03:00.0) */
    char            sys_path[256];  /* Path to device (/sys/device/... */
    unsigned short  vendor;
    unsigned short  device;
    unsigned short  subsystem_vendor;
    unsigned short  subsystem_device;
    unsigned int    class;          /* 3 bytes: (base,sub,prog-if) */
    struct rtapi_pci_resource   
                    resource[6];    /* Device BARs */    
    void *driver_data;              /* Data private to the driver */
};

typedef struct rtapi_pm_message {
    int event;
} rtapi_pm_message_t;

struct rtapi_pci_driver {
//    struct list_head node;
    char *name;
    const struct rtapi_pci_device_id *id_table;   /* must be non-NULL for probe to be called */
    int  (*probe)  (struct rtapi_pci_dev *dev, const struct rtapi_pci_device_id *id);   /* New device inserted */
    void (*remove) (struct rtapi_pci_dev *dev);   /* Device removed (NULL if not a hot-plug capable driver) */
    //int  (*suspend) (struct rtapi_pci_dev *dev, pm_message_t state);      /* Device suspended */
    //int  (*suspend_late) (struct rtapi_pci_dev *dev, pm_message_t state);
    //int  (*resume_early) (struct rtapi_pci_dev *dev);
    //int  (*resume) (struct rtapi_pci_dev *dev);                   /* Device woken up */
    //void (*shutdown) (struct rtapi_pci_dev *dev);
//    struct pci_error_handlers *err_handler;
//    struct device_driver    driver;
//    struct pci_dynids dynids;
};


int rtapi_pci_register_driver(struct rtapi_pci_driver *driver);
void rtapi_pci_unregister_driver(struct rtapi_pci_driver *driver);
int rtapi_pci_enable_device(struct rtapi_pci_dev *dev);
int rtapi_pci_disable_device(struct rtapi_pci_dev *dev);

#define rtapi_pci_resource_start(dev, bar)    ((dev)->resource[(bar)].start)
#define rtapi_pci_resource_end(dev, bar)      ((dev)->resource[(bar)].end)
#define rtapi_pci_resource_flags(dev, bar)    ((dev)->resource[(bar)].flags)
#define rtapi_pci_resource_len(dev, bar) \
        ((rtapi_pci_resource_start((dev), (bar)) == 0 &&    \
          rtapi_pci_resource_end((dev), (bar)) ==           \
          rtapi_pci_resource_start((dev), (bar))) ? 0 :     \
                                                            \
         (rtapi_pci_resource_end((dev), (bar)) -            \
          rtapi_pci_resource_start((dev), (bar)) + 1))

void *rtapi_pci_ioremap_bar(struct rtapi_pci_dev *pdev, int bar);
inline void rtapi_iounmap(volatile void rtapi_iomem *addr);

static inline const char *rtapi_pci_name(const struct rtapi_pci_dev *pdev)
{
    return pdev->dev_name;
}

static inline void rtapi_pci_set_drvdata(struct rtapi_pci_dev *pdev, void *data)
{
    pdev->driver_data = data;
}

/** rtapi_pci_get_device - Find a PCI device
 * @vendor: The vendor ID to search for
 * @device: The device ID to search for
 * @from: The device to start searching from. Can be NULL.
 */
extern
struct rtapi_pci_dev * rtapi_pci_get_device(__u16 vendor, __u16 device,
					   struct rtapi_pci_dev *from);

/** rtapi_pci_put_device - Free a PCI device obtained by rtapi_pci_get_device() */
extern
void rtapi_pci_put_device(struct rtapi_pci_dev *dev);

/** rtapi_pci_ioremap - Remap I/O memory
 * Returns NULL on error.
 * @dev: The device
 * @bar: The PCI BAR to remap.
 * @size: The size of the mapping.
 */
extern
void * rtapi_pci_ioremap(struct rtapi_pci_dev *dev, int bar, size_t size);

/** rtapi_pci_iounmap - Unmap an MMIO region
 * @dev: The device
 * @mmio: The MMIO region obtained by rtapi_pci_ioremap()
 */
extern
void rtapi_pci_iounmap(struct rtapi_pci_dev *dev, void *mmio);
#endif
#endif // RTAPI_PCI_H
