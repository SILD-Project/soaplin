/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  spinlock.c - Spinlock implementation.
 */

#include <lib/spinlock.h>

void sl_acquire(spinlock_t volatile *plock)
{
    while (!__sync_bool_compare_and_swap(plock, 0, 1))
    {
        while (*plock)
        {
            asm("pause");
        }
    }
}

void sl_release(spinlock_t volatile *plock)
{
    __sync_lock_release(plock);
}

