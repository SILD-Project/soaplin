#include "deps/limine.h"
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

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

bool limine_ensure_baserev() { return LIMINE_BASE_REVISION_SUPPORTED; }

limine_fb_t *limine_get_fb(int id) {
    if (!framebuffer_request.response)
        return NULL;

    if (id >= (int32_t)framebuffer_request.response->framebuffer_count) // Limine, WHY putting the FB count as a 64-bit integer??? I never seen someone with 0xFFFFFFFFFFFFFFFF screens
        return NULL;
    return framebuffer_request.response->framebuffers[id];
}