/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  smp.c - x86_64 Symetric Multiprocessing implementation
 */

#include "arch/cpu.h"
#include <boot/limine.h>
#include <deps/limine.h>
#include <lib/log.h>
#include <stdint.h>

uint32_t bootstrap_lapic_id;

// TODO: initialize the CPUs

void cpu_init_smp() {
    struct limine_mp_response *smp = limine_get_smp();
    bootstrap_lapic_id = smp->bsp_lapic_id;

    /*for (uint64_t i = 0; i < smp->cpu_count; i++) {
        trace("smp: Starting CPU %d\n", i);
    }*/
}
