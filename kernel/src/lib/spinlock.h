#pragma once

#include <stdint.h>

typedef struct spinlock {
  volatile int locked;
} spinlock_t;

inline void spinlock_acquire(spinlock_t *lock) {
  while (__sync_lock_test_and_set(&(lock)->locked, 1))
    while ((lock)->locked)
      __asm__ volatile("pause");
}

inline void spinlock_release(spinlock_t *lock) {
  __sync_lock_release(&(lock)->locked);
}