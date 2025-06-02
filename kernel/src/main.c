/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  main.c - Kernel entry point and initialization.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <acpi/acpi.h>
#include <acpi/madt.h>
#include <arch/cpu.h>
#include <boot/limine.h>
#include <config.h>
#include <dev/tty.h>
#include <lib/ansi.h>
#include <lib/log.h>
#include <lib/logoutputs_sk.h>
#include <mm/memop.h>
#include <mm/pmm.h>
#include "arch/x86_64/smp.h"
#include "mm/paging.h"
#include "mm/vma.h"

void kmain(void) {
    tty_init();
    log_init(LOGLEVEL_TRACE);
    log_register_output(sklogoutput_tty);

    limine_bootinfo_t *bi = limine_get_bootinfo();
    info("%s %s-%s (booted using %s %s, with firmware type %d)\n", KERNEL_NAME, KERNEL_VER,
        bi->arch, bi->bl_name, bi->bl_ver, bi->fw_type);

    arch_init_stage1();

    pmm_init();
    pg_init();
    
    acpi_init();
    madt_init();

    cpu_init_apic();
    cpu_init_smp();
    cpu_init_timer();

    
    cpu_enable_ints(1);
    //lapic_ipi(bootstrap_lapic_id, 32);

    while (1)
        ;;
    // We're done, just hang... for now.
    //hcf();
}
