# Soaplin
The Soaplin kernel is a new Unix-like operating system kernel.

## Features
* x86_64 support (kinda)
* Memory management (VMM/PMM/VMA/kmalloc)
* Simple pre-emptive scheduler
* Ring 3 (user mode) support
* ELF loader
* ACPI
* Symetric Multiprocessing (It runs code, but the scheduler is running on 1 processor.)

## In the works
* Virtual File System
* FPU support

## To come
* RTC support
* EXT2 driver 
* FAT32 driver
* TAR-based init ram disk
* Video driver for Bochs graphics adapter, and the VMware display adapter.

## Known bugs
* If a user process calls the syscall handler by using interrupts, the system may crash due to a Page Fault.

## Building
To build Soaplin, you must ensure you have these:
* The Netwide Assembler (nasm)
* A compiler (both GCC & Clang works!) (In the future, a custom toolchain would be built.)
* A linker (both GNU LD & LLVM linker works too!)

Clone the repo, and run "make" in it!

## Acknowledgements
Thanks to all these people :heart:
* Limine contributors: The Limine bootloader, used for kicking up Soaplin
* Kevin Alavik (kevinalavik): VMAs implementation
* Astrido (asterd-og): IOAPIC/LAPIC (from ZanOS)