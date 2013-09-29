
//
//    Copyright (C) 2013 Andy Pugh

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

// A driver for the Hostmot2 HM2_DPLL module that allows pre-triggering of some
// other modules. 


#include <linux/slab.h>

#include "rtapi.h"
#include "rtapi_string.h"
#include "rtapi_math.h"

#include "hal.h"

#include "hal/drivers/mesa-hostmot2/hostmot2.h"

int hm2_hm2dpll_parse_md(hostmot2_t *hm2, int md_index) {

    hm2_module_descriptor_t *md = &hm2->md[md_index];
    int r;

    //
    // some standard sanity checks
    //

    if (!hm2_md_is_consistent_or_complain(hm2, md_index, 0, 7, 4, 0x0000)) {
        HM2_ERR("inconsistent Module Descriptor!\n");
        return -EINVAL;
    }


    if (hm2->config.num_hm2dplls == 0) return 0;

    if (hm2->config.num_hm2dplls > md->instances) {
        hm2->hm2dpll.num_instances = md->instances;
        HM2_ERR( "There are only %d dplls on this board type, using %d\n",
                md->instances, md->instances );
    } else if (hm2->config.num_hm2dplls == -1) {
        hm2->hm2dpll.num_instances = md->instances;
    } else hm2->hm2dpll.num_instances = hm2->config.num_hm2dplls;

    //
    // looks good, start initializing
    //

    hm2->hm2dpll.clock_frequency = md->clock_freq;
    
    hm2->hm2dpll.base_rate_addr = md->base_address + 0 * md->register_stride;
    hm2->hm2dpll.phase_error_addr = md->base_address + 1 * md->register_stride;
    hm2->hm2dpll.control_reg0_addr = md->base_address + 2 * md->register_stride;
    hm2->hm2dpll.control_reg1_addr = md->base_address + 3 * md->register_stride;
    hm2->hm2dpll.timer_12_addr = md->base_address + 4 * md->register_stride;
    hm2->hm2dpll.timer_34_addr = md->base_address + 5 * md->register_stride;
    hm2->hm2dpll.hm2_hm2dpll_sync_addr = md->base_address + 6 * md->register_stride;
    
    // export to HAL
    hm2->hm2dpll.pins = hal_malloc(sizeof(hm2_hm2dpll_pins_t));

    r = hal_pin_float_newf(HAL_IN, &(hm2->hm2dpll.pins->time1_ms),
            hm2->llio->comp_id, "%s.hm2dpll.01.timer-ms", hm2->llio->name);
    r += hal_pin_float_newf(HAL_IN, &(hm2->hm2dpll.pins->time2_ms),
            hm2->llio->comp_id, "%s.hm2dpll.02.timer-ms", hm2->llio->name);
    r += hal_pin_float_newf(HAL_IN, &(hm2->hm2dpll.pins->time3_ms),
            hm2->llio->comp_id, "%s.hm2dpll.03.timer-ms", hm2->llio->name);
    r += hal_pin_float_newf(HAL_IN, &(hm2->hm2dpll.pins->time4_ms),
            hm2->llio->comp_id, "%s.hm2dpll.04.timer-ms", hm2->llio->name);
    r += hal_pin_float_newf(HAL_IN, &(hm2->hm2dpll.pins->base_freq),
            hm2->llio->comp_id, "%s.hm2dpll.base-freq-khz", hm2->llio->name);
    r += hal_pin_s32_newf(HAL_OUT, &(hm2->hm2dpll.pins->phase_error),
            hm2->llio->comp_id, "%s.hm2dpll.phase-error", hm2->llio->name);
    r += hal_pin_u32_newf(HAL_IN, &(hm2->hm2dpll.pins->time_const),
            hm2->llio->comp_id, "%s.hm2dpll.time-const", hm2->llio->name);
    r += hal_pin_u32_newf(HAL_IN, &(hm2->hm2dpll.pins->plimit),
            hm2->llio->comp_id, "%s.hm2dpll.plimit", hm2->llio->name);
    r += hal_pin_u32_newf(HAL_IN, &(hm2->hm2dpll.pins->filter),
            hm2->llio->comp_id, "%s.hm2dpll.filter", hm2->llio->name);
    r += hal_pin_u32_newf(HAL_OUT, &(hm2->hm2dpll.pins->ddssize),
            hm2->llio->comp_id, "%s.hm2dpll.ddsize", hm2->llio->name);
    r += hal_pin_u32_newf(HAL_IN, &(hm2->hm2dpll.pins->prescale),
            hm2->llio->comp_id, "%s.hm2dpll.prescale", hm2->llio->name);
    if (r < 0) {
        HM2_ERR("error adding hm2_dpll timer pins, Aborting\n");
        goto fail0;
    }
    *hm2->hm2dpll.pins->prescale = 1;
    *hm2->hm2dpll.pins->base_freq = -1; // An indication it needs init

    r = hm2_register_tram_write_region(hm2, hm2->hm2dpll.hm2_hm2dpll_sync_addr,
            sizeof(u32), &hm2->hm2dpll.hm2_hm2dpll_sync_reg);
    if (r < 0) {
        HM2_ERR("Error registering tram synch write. Aborting\n");
        goto fail0;
    }
    r = hm2_register_tram_read_region(hm2, hm2->hm2dpll.control_reg1_addr,
            sizeof(u32), &hm2->hm2dpll.control_reg1_read);
    if (r < 0) {
        HM2_ERR("Error registering hm2dpll control reg 1. Aborting\n");
        goto fail0;
    }
    r = hm2_register_tram_read_region(hm2, hm2->hm2dpll.phase_error_addr,
            sizeof(u32), &hm2->hm2dpll.phase_error_reg_read);
    if (r < 0) {
        HM2_ERR("Error registering hm2dpll phase error. Aborting\n");
        goto fail0;
    }


    return hm2->hm2dpll.num_instances;

    fail0:
    return r;

}

void hm2_hm2dpll_process_tram_read(hostmot2_t *hm2, long period){
    hm2_hm2dpll_pins_t *pins;
    
    if (hm2->hm2dpll.num_instances == 0) return;
    
     pins = hm2->hm2dpll.pins;
    
    *pins[0].phase_error = *hm2->hm2dpll.phase_error_reg_read;
    *pins->ddssize = *hm2->hm2dpll.control_reg1_read & 0xFF;
    *pins->phase_error = *hm2->hm2dpll.phase_error_reg_read;
}

void hm2_hm2dpll_write(hostmot2_t *hm2, long period) {
    hm2_hm2dpll_pins_t *pins;
    double period_ms = period / 1000;
    u32 buff;
    
    if (hm2->hm2dpll.num_instances == 0) return;
    
    pins = hm2->hm2dpll.pins;
    
    if (*pins->base_freq < 0 ) {
        *pins->base_freq = 1000.0/period_ms;
    }

    if (*pins->prescale != 0){
        buff = (u32)((*pins->base_freq * 1000.0 * (1LL << *pins->ddssize) 
                / (hm2->hm2dpll.clock_frequency * *pins->prescale)));
    } else {
        buff = 0;
    }
    
    if (buff != hm2->hm2dpll.base_rate_written){
        hm2->llio->write(hm2->llio,
                hm2->hm2dpll.base_rate_addr,
                &buff,
                sizeof(u32));
        HM2_PRINT("setting base rate to %08x. Clock frequency %i base-freq %i\n", buff, hm2->hm2dpll.clock_frequency, (u32)*hm2->hm2dpll.pins->base_freq);
        hm2->hm2dpll.base_rate_written= buff;
    }
    buff = (u32)(*pins->prescale << 24 
                | *pins->plimit);
    if (buff != hm2->hm2dpll.control_reg0_written){
        hm2->llio->write(hm2->llio,
                hm2->hm2dpll.control_reg0_addr,
                &buff,
                sizeof(u32));
        HM2_PRINT("setting ctrl reg 0 to %08x\n", buff);
        hm2->hm2dpll.control_reg0_written= buff;
    }
    buff = (u32)(*pins->time_const << 16);
    if (buff != hm2->hm2dpll.control_reg1_written){
        hm2->llio->write(hm2->llio,
                hm2->hm2dpll.control_reg1_addr,
                &buff,
                sizeof(u32));
        HM2_PRINT("setting control reg 1 to %08x\n", buff);
        hm2->hm2dpll.control_reg1_written= buff;
    }
    buff = (u32)((*hm2->hm2dpll.pins->time2_ms / period_ms) * 0x10000) << 16
         | (u32)((*hm2->hm2dpll.pins->time1_ms / period_ms) * 0x10000);
    if (buff != hm2->hm2dpll.timer_12_written){
        hm2->llio->write(hm2->llio,
                hm2->hm2dpll.timer_12_addr,
                &buff,
                sizeof(u32));
        HM2_PRINT("setting timer01 to %08x\n", buff);
        hm2->hm2dpll.timer_12_written = buff;
    }
    buff = (u32)((*hm2->hm2dpll.pins->time4_ms / period_ms) * 0x10000) << 16
         | (u32)((*hm2->hm2dpll.pins->time3_ms / period_ms) * 0x10000);
    if (buff != hm2->hm2dpll.timer_34_written){
        hm2->llio->write(hm2->llio,
                hm2->hm2dpll.timer_34_addr,
                &buff,
                sizeof(u32));
        HM2_PRINT("setting timer23 to %08x\n", buff);
        hm2->hm2dpll.timer_34_written = buff;
    }
}

int hm2_hm2dpll_force_write(hostmot2_t *hm2) {
    int i;
    if (hm2->hm2dpll.num_instances == 0) return 0;
    for (i = 0; i < hm2->absenc.num_chans; i ++){
        hm2_sserial_remote_t *chan = &hm2->absenc.chans[i];
        switch (chan->myinst){
        case HM2_GTAG_SSI:
            chan->params->timer_num = 1;
            break;
        case HM2_GTAG_BISS:
            if (hm2->hm2dpll.num_instances > 1){
                chan->params->timer_num = 2;
            } else {
                chan->params->timer_num = 1;
            }
            break;
        case HM2_GTAG_FABS:
            if (hm2->hm2dpll.num_instances > 2){
                chan->params->timer_num = 3;
            } else {
                chan->params->timer_num = 1;
            }
            break;
        }
   /* Other triggerable component types should be added here */
    }
    return 0;
}


void hm2_hm2dpll_cleanup(hostmot2_t *hm2) {
    // Should all be handled by the HAL housekeeping
}
