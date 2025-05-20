/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  smp.c - x86_64 Symetric Multiprocessing implementation
 */

#include <boot/limine.h>
#include <deps/limine.h>
#include <stdint.h>

uint32_t bootstrap_lapic_id;

void cpu_init_smp() {
    struct limine_mp_response *smp = limine_get_smp();
    bootstrap_lapic_id = smp->bsp_lapic_id;
}
