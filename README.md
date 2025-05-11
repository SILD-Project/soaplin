# Soaplin
The Soaplin kernel is a new Unix-like operating system kernel.

## Features
* x86_64 support (kinda)
* Memory management (VMM/PMM)
* Simple pre-emptive scheduler
* Ring 3 (user mode) support
* ELF loader

## In the works
* Virtual File System
* FPU support

## To come
* RTC support
* EXT2 driver 
* FAT32 driver
* CPIO-based init ram disk
* Video driver for Bochs graphics adapter, and the VMware display adapter.

## Known bugs
* If a user process calls the syscall handler by using interrupts, the system may crash due to a Page Fault.