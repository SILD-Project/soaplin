#include "arch//x86_64/smp.h"
#include "lib/spinlock.h"
#include "sys/log.h"
#include "limine.h"
#include <stdint.h>

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_mp_request
    smp_request = {.id = LIMINE_MP_REQUEST, .revision = 0};

uint32_t bootstrap_lapic_id = 0;
uint32_t smp_cpu_count = 0; // Number of processors
spinlock_t smp_lock = { 0 };

uint32_t ctr = 0;

void smp_entry(struct limine_mp_info* smp_info) {
    //spinlock_acquire(&smp_lock);

    log("smp - CPU %d started (LAPIC ID: %d)\n", smp_info->processor_id, smp_info->lapic_id);
    __atomic_fetch_add(&ctr, 1, __ATOMIC_SEQ_CST);

    while (1)
        ;;
    //spinlock_release(&smp_lock);
}

void smp_init() {
    bootstrap_lapic_id = smp_request.response->bsp_lapic_id;
    smp_cpu_count = smp_request.response->cpu_count;

    log("smp - detected %d CPUs\n", smp_cpu_count);

    for (uint64_t i = 0; i < smp_cpu_count; i++) {
        if (smp_request.response->cpus[i]->lapic_id != bootstrap_lapic_id) {
            uint32_t old_ctr = __atomic_load_n(&ctr, __ATOMIC_SEQ_CST);

            __atomic_store_n(&smp_request.response->cpus[i]->goto_address, smp_entry, __ATOMIC_SEQ_CST);

            while (__atomic_load_n(&ctr, __ATOMIC_SEQ_CST) == old_ctr)
                ;
        } else {
            log("smp - CPU %d is the bootstrap processor (LAPIC ID: %d)\n", i, smp_request.response->cpus[i]->lapic_id);
        }
    }

    log("smp - initialized\n");
}