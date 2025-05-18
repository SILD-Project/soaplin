#pragma once

#include <deps/limine.h>
#include <stdbool.h>

typedef struct limine_framebuffer limine_fb_t;

// Ensure that the used boot loader supports the kernel's LBP revision
bool limine_ensure_baserev();

// Get a framebuffer
limine_fb_t *limine_get_fb(int id);