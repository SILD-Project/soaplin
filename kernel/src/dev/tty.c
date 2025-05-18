
#include <boot/limine.h>
#include <deps/flanterm/backends/fb.h>
#include <deps/flanterm/flanterm.h>
#include <lib/string.h>

struct flanterm_context *tty0_ctx; 

void tty_init() {
    limine_fb_t *fb0 = limine_get_fb(0);

    tty0_ctx = flanterm_fb_init(
            NULL,
            NULL,
            fb0->address, fb0->width, fb0->height, fb0->pitch,
            fb0->red_mask_size, fb0->red_mask_shift,
            fb0->green_mask_size, fb0->green_mask_shift,
            fb0->blue_mask_size, fb0->blue_mask_shift,
            NULL,
            NULL, NULL,
            NULL, NULL,
            NULL, NULL,
            NULL, 0, 0, 1,
            0, 0,
            0
    );
}

void tty_write_raw(char *str) {
    flanterm_write(tty0_ctx, str, strlen(str));
}