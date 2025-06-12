/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  limine.c - Limine bootloader interface implementation.
 */

#include "lib/log.h"
#include <stdbool.h>
#include <stddef.h>
#include <boot/limine.h>
#include <deps/limine.h>
#include <proc/exec/elf.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_bootloader_info_request bl_info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_firmware_type_request firmware_type_req = {
    .id = LIMINE_FIRMWARE_TYPE_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_executable_address_request kaddr_req = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_executable_file_request execfile_req = {
    .id = LIMINE_EXECUTABLE_FILE_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_paging_mode_request pgmode_req = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .mode = LIMINE_PAGING_MODE_X86_64_4LVL,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request rsdp_req = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_mp_request smp_req = {
    .id = LIMINE_MP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_module_request mod_req = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 3
};


__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;


static limine_bootinfo_t __limine_bootinfo;


bool limine_ensure_baserev() { return LIMINE_BASE_REVISION_SUPPORTED; }

limine_fb_t *limine_get_fb(int id) {
    if (!framebuffer_request.response)
        return NULL;

    if (id >= (int32_t)framebuffer_request.response->framebuffer_count) // Limine, WHY putting the FB count as a 64-bit integer??? I never seen someone with 0xFFFFFFFFFFFFFFFF screens
        return NULL;
    return framebuffer_request.response->framebuffers[id];
}

struct limine_memmap_response *limine_get_memmap() {
    return memmap_req.response;
}

limine_bootinfo_t *limine_get_bootinfo() {
    __limine_bootinfo.bl_name = bl_info_request.response->name;
    __limine_bootinfo.bl_ver = bl_info_request.response->version;
    __limine_bootinfo.fw_type = firmware_type_req.response->firmware_type;

#if defined(__x86_64__)
    __limine_bootinfo.arch = "x86_64";
#elif defined(__aarch64__)
    __limine_bootinfo.arch = "aarch64";
#elif defined(__riscv)
    __limine_bootinfo.arch = "riscv";
#elif defined(__loongarch64)
    __limine_bootinfo.arch = "loongarch64";
#endif

    return &__limine_bootinfo;
}

uint64_t limine_get_hhdm_offset() { return hhdm_req.response->offset; }
uint64_t limine_get_kernel_vaddr() { return kaddr_req.response->virtual_base; }
uint64_t limine_get_kernel_paddr() { return kaddr_req.response->physical_base; }
uint64_t limine_get_kernel_ehdr_addr() { return (uint64_t)execfile_req.response->executable_file->address; }
uint64_t limine_get_rsdp() { return rsdp_req.response->address + limine_get_hhdm_offset(); }
struct limine_mp_response *limine_get_smp() { return smp_req.response; }

struct limine_file *limine_get_module(int no) {
    if (mod_req.response == NULL) {
        trace("limine: mod_req.response is NULL (why?)\n");
        while (1)
            ;;
    }
    if (mod_req.response->module_count < (uint64_t)no) {
        trace("limine: mod_req.response->module_count < (uint64_t)no (modcount: %d)\n", mod_req.response->module_count);
        return NULL;
    }

    return mod_req.response->modules[no];
}