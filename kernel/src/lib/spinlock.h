#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct spinlock {
    volatile int locked;
} spinlock_t;

static inline void spinlock_acquire(spinlock_t *lock) {
    //uint64_t timeout = 1000000;  // Adjust this value based on your needs
    
    for (;;) {
        if (__atomic_exchange_n(&lock->locked, 1, __ATOMIC_ACQUIRE) == 0) {
            return;
        }
        
        while (__atomic_load_n(&lock->locked, __ATOMIC_RELAXED)) {
            /**if (--timeout == 0) {
                // Force unlock after too many attempts
                __atomic_store_n(&lock->locked, 0, __ATOMIC_RELEASE);
                continue;
            }**/
            __asm__ volatile("pause" ::: "memory");
        }
    }
}

static inline void spinlock_release(spinlock_t *lock) {
    __atomic_store_n(&lock->locked, 0, __ATOMIC_RELEASE);
}