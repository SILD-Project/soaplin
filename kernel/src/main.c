#include "exec/elf.h"
#include "font.h"
#include "mm/pmm.h"
#include "mm/vma.h"
#include "mm/vmm.h"
#include "mm/liballoc/liballoc.h"
#include "mm/memop.h"
#include "rt.h"
#include "sched/sched.h"
#include "sys/arch/x86_64/cpuid.h"
#include "sys/arch/x86_64/io.h"
#include "sys/arch/x86_64/pit.h"
#include "sys/arch/x86_64/rtc.h"
#include "sys/arch/x86_64/sse.h"
#include <sys/log.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

#include <sys/printf.h>
#include <sys/arch/x86_64/gdt.h>
#include <sys/arch/x86_64/idt.h>

#define SSFN_IMPLEMENTATION        /* use the special renderer for 32 bit truecolor packed pixels */
#include <sys/gfx/ssfn.h>

// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

/*__attribute__((used, section(".limine_requests")))
static volatile struct limine_entry_point_request entrypoint_request = {
    .id = LIMINE_ENTRY_POINT_REQUEST,
    .revision = 3
};*/
// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;


// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
#if defined (__x86_64__)
        asm ("hlt");
#elif defined (__aarch64__) || defined (__riscv)
        asm ("wfi");
#elif defined (__loongarch64)
        asm ("idle 0");
#endif
    }
}



int init() {
    asm("int $0x80");
    while (1)
        ;;
}

struct limine_framebuffer *fb;

char kstack[8192];

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    /*if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }*/

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    fb = framebuffer;

    rt_context ctx;
    ctx.framebuffer = fb->address;
    ctx.framebuffer_width = fb->width;
    ctx.framebuffer_height = fb->height;
    rt_init(ctx);

    printf("\n  Soaplin 1.0-sild is booting up your computer...\n\n");
    //printf("Physical kernel EP: %p", entrypoint_request.entry);
    
    gdt_init(&kstack[8192]);
    idt_init();

    sse_init();

    pmm_init();
    vmm_init();
    kernel_vma_context = vma_create_context(vmm_kernel_pm);
    if (!kernel_vma_context) {
        log("kernel - vma ctx creation failed. halting\n");
        asm("cli");
        while (1)
            asm("hlt");
    }

    char *a = kmalloc(1);
    *a = 32;
    log("Allocated 1 byte at 0x%.16llx\n", (uint64_t)a);
    kfree(a);

    pit_init(1000);
    sched_init();
    //user_init();

    struct limine_file *f = module_request.response->modules[0];
    log("kmain - %s\n", f->path);

    elf_load((char*)f->address);

    memset(framebuffer->address, 0, framebuffer->pitch * framebuffer->height);

    ssfn_t sctx = { 0 };                                 /* the renderer context */
    ssfn_buf_t buf = {                                  /* the destination pixel buffer */
        .ptr = framebuffer->address,                      /* address of the buffer */
        .w = framebuffer->width,                             /* width */
        .h = framebuffer->height,                             /* height */
        .p = framebuffer->pitch,                         /* bytes per line */
        .x = 100,                                       /* pen position */
        .y = 100,
        .fg = 0xFF808080                                /* foreground color */
    };
    
    /* add one or more fonts to the context. Fonts must be already in memory */
    ssfn_load(&sctx, &NotoRegular);
    
    /* select the typeface to use */
    ssfn_select(&sctx,
        SSFN_FAMILY_SANS, NULL,                        /* family */
        SSFN_STYLE_REGULAR | SSFN_STYLE_UNDERLINE,      /* style */
        64                                              /* size */
    );
    
    /* rasterize the first glyph in an UTF-8 string into a 32 bit packed pixel buffer */
    /* returns how many bytes were consumed from the string */
    ssfn_render(&sctx, &buf, "Hello World.");

    /* free resources */
    ssfn_free(&sctx); 
    

    //uint8_t *mem = pmm_request_page();
    //mem[0] = 0xCD;
    //mem[1] = 0x80;
    //mem[2] = 0xF4;

    //mem[3] = 0xFE;
    //pagemap_t* pm = vmm_alloc_pm();
    //vmm_map(pm, 0x1000, (uint64_t)mem, VMM_PRESENT | VMM_USER);
    //sched_create("Init", 0x1000, pm, SCHED_USER_PROCESS);

    //log("kernel - Soaplin initialized sucessfully.\n");
    while (1)
        ;;//__asm__ volatile ("hlt");
}
