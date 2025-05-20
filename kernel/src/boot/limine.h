/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  limine.h - Limine bootloader interface declarations.
 */

#pragma once

#include <deps/limine.h>
#include <stdbool.h>

typedef struct limine_framebuffer limine_fb_t;

typedef struct {
    char *bl_name;
    char *bl_ver;
    char *arch;
    int fw_type;
} limine_bootinfo_t;

// Ensure that the used boot loader supports the kernel's LBP revision
bool limine_ensure_baserev();

// Get a framebuffer
limine_fb_t *limine_get_fb(int id);

// Get some informations about how the kernel was booted
limine_bootinfo_t *limine_get_bootinfo();

// Get the memory map.
struct limine_memmap_response *limine_get_memmap();

uint64_t limine_get_hhdm_offset();
uint64_t limine_get_kernel_vaddr();
uint64_t limine_get_kernel_paddr();
uint64_t limine_get_kernel_ehdr_addr();
uint64_t limine_get_rsdp();
struct limine_mp_response *limine_get_smp();