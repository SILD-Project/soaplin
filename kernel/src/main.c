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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/arch/x86_64/fpu.h>
#include <sys/arch/x86_64/gdt.h>
#include <sys/arch/x86_64/idt.h>
#include <sys/gfx/flanterm/backends/fb.h>
#include <sys/gfx/flanterm/flanterm.h>
#include <sys/log.h>
#include <sys/printf.h>

// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((
    used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((
    used,
    section(
        ".limine_requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_module_request
    module_request = {.id = LIMINE_MODULE_REQUEST, .revision = 0};

/*__attribute__((used, section(".limine_requests")))
static volatile struct limine_entry_point_request entrypoint_request = {
    .id = LIMINE_ENTRY_POINT_REQUEST,
    .revision = 3
};*/
// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used,
               section(".limine_requests_"
                       "start"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((
    used,
    section(
        ".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

// Halt and catch fire function.
static void hcf(void) {
  for (;;) {
#if defined(__x86_64__)
    asm("hlt");
#elif defined(__aarch64__) || defined(__riscv)
    asm("wfi");
#elif defined(__loongarch64)
    asm("idle 0");
#endif
  }
}

struct limine_framebuffer *fb;
struct flanterm_context *ft_ctx;
uint32_t fg = 0xFFFFFF;

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
  if (framebuffer_request.response != NULL) {

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer =
        framebuffer_request.response->framebuffers[0];
    fb = framebuffer;

    ft_ctx = flanterm_fb_init(
        NULL, NULL, framebuffer->address, framebuffer->width,
        framebuffer->height, framebuffer->pitch, framebuffer->red_mask_size,
        framebuffer->red_mask_shift, framebuffer->green_mask_size,
        framebuffer->green_mask_shift, framebuffer->blue_mask_size,
        framebuffer->blue_mask_shift, NULL, NULL, NULL, NULL, &fg, NULL, NULL,
        VGA8, 8, 16, 0, 0, 0, 0);
  }

  printf("\n  Soaplin 1.0-sild is booting up your computer...\n\n");
  // printf("Physical kernel EP: %p", entrypoint_request.entry);

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

  // acpi_init();
  syscall_init();
  pit_init(1000);
  sched_init();
  // user_init();

  struct limine_file *f = module_request.response->modules[0];
  log("kmain - %s\n", f->path);

  program_t *prog = elf_load((char *)f->address, 1);

  sched_create("Init", prog->entry, prog->pm, SCHED_USER_PROCESS);

  log("kernel - Soaplin initialized sucessfully.\n");
  while (1)
    ;
  ; //__asm__ volatile ("hlt");
}
