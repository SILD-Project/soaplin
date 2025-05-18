/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  limine.c - Limine bootloader interface implementation.
 */

#include "deps/limine.h"
#include "limine.h"
#include <stdbool.h>
#include <stddef.h>
#include <boot/limine.h>

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

uint64_t limine_get_hhdm_offset()
{
    return hhdm_req.response->offset;
}