/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  spinlock.h - Spinlock definitions.
 */

#pragma once

typedef unsigned int spinlock_t;

void sl_acquire(spinlock_t volatile *plock);
void sl_release(spinlock_t volatile *plock);