#include "exec/elf.h"
#include "exec/exec.h"
#include "mm/liballoc/liballoc.h"
#include "mm/pmm.h"
#include "mm/vma.h"
#include "mm/vmm.h"
#include "sched/sched.h"
#include "sys/acpi.h"
#include "sys/arch/x86_64/pit.h"
#include "sys/arch/x86_64/sse.h"
#include "sys/syscall.h"
#include <font.h>
#include <limine.h>
#include <mm/memop.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/arch/x86_64/fpu.h>
#include <sys/arch/x86_64/gdt.h>
#include <sys/arch/x86_64/idt.h>
#include <sys/errhnd/panic.h>
#include <sys/gfx/flanterm/backends/fb.h>
#include <sys/gfx/flanterm/flanterm.h>
#include <sys/log.h>
#include <sys/printf.h>
#include <fs/vfs.h>

__attribute__((
    used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);

__attribute__((
    used,
    section(
        ".limine_requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_module_request
    module_request = {.id = LIMINE_MODULE_REQUEST, .revision = 0};

__attribute__((used,
               section(".limine_requests_"
                       "start"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((
    used,
    section(
        ".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

struct limine_framebuffer *fb;
struct flanterm_context *ft_ctx;

char kstack[8192];

void kmain(void) {
  if (framebuffer_request.response != NULL) {

    struct limine_framebuffer *framebuffer =
        framebuffer_request.response->framebuffers[0];
    fb = framebuffer;

    ft_ctx = flanterm_fb_init(
        NULL, NULL, framebuffer->address, framebuffer->width,
        framebuffer->height, framebuffer->pitch, framebuffer->red_mask_size,
        framebuffer->red_mask_shift, framebuffer->green_mask_size,
        framebuffer->green_mask_shift, framebuffer->blue_mask_size,
        framebuffer->blue_mask_shift, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        VGA8, 8, 16, 0, 0, 0, 0);
  }

  printf("\n  Soaplin 1.0-sild is booting up your computer...\n\n");

  gdt_init(&kstack[8192]);
  idt_init();
  fpu_activate();
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
  
  syscall_init();
  pit_init(1000);
  sched_init();
  
  //vfs_init();



  panic("No working initialization program found. (This is normal due to "
        "Soaplin's current state, so please do not report this as a bug)");

  log("kernel - Soaplin initialized sucessfully.\n");

  while (1)
    ;
  ;
}
