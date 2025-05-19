# Soaplin (rewrite)
This is a carefully rewritten version of Soaplin, that should be held with super glue instead of
0.99$ duct tape bought on TEMU.

## Features
* Support for compiling the kernel on armv8, RISC-V, and LoongArch64 (even though the kernel doesn't do anything on these architectures)
* Free list PMM with lazy loading
* x86_64 paging

## To do
* ACPI & MADT
* IOAPIC / LAPIC
* SMP
* Multithreaded scheduler
* VFS
* Tar file system