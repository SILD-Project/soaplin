/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  gdt.c - x86_64 Global Descriptor Table implementation.
 */

#include "lib/log.h"
#if defined(__x86_64__)

#include <stdint.h>
#include <arch/x86_64/gdt.h>
#include <mm/memop.h>

gdt_table gdt = {{
    0x0000000000000000, // 0x0, null

    0x00af9b000000ffff, // 0x8, 64-bit code
    0x00af93000000ffff, // 0x10, 64-bit data

    0x00affb000000ffff, // 0x18, usermode 64-bit code
    0x00aff3000000ffff  // 0x20, usermode 64-bit data
}, {}};

tssr tss_list[255];
static uint64_t __gdt_kstack[4096];

extern void gdt_reload_segments();

void gdt_init() {
    memset(&tss_list[0], 0, sizeof(tssr));
    tss_list[0].rsp[0] = (uint64_t)__gdt_kstack;
    tss_list[0].iopb = sizeof(tssr);
    uintptr_t tss = (uintptr_t)&tss_list[0];

    gdt.tss_entry = (tss_entry){
        .length = sizeof(tss_entry),
        .base = (uint16_t)(tss & 0xFFFF),
        .base1 = (uint8_t)((tss >> 16) & 0xFF),
        .flags = 0x89,
        .flags1 = 0,
        .base2 = (uint8_t)((tss >> 24) & 0xFF),
        .base3 = (uint32_t)(tss >> 32),
        .resv = 0,
    };

    gdtr gdtr = {
        .size = (sizeof(gdt_table)) - 1,
        .address = (uint64_t)&gdt
    };

    __asm__ volatile("lgdt %0\n\t" : : "m"(gdtr) : "memory");
    __asm__ volatile("ltr %0\n\t" : : "r"((uint16_t)0x28)); // 0x20 (last GDT entry) + 0x8 (size of a GDT entry)

    gdt_reload_segments();

    debug("arch: GDT & TSS initialized.\n");
}

#endif