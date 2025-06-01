# Nuke built-in rules and variables.
MAKEFLAGS += -rR
.SUFFIXES:

# This is the name that our final executable will have.
# Change as needed.
override OUTPUT := kernel

# Target architecture to build for. Default to x86_64.
ARCH := x86_64

# Install prefix; /usr/local is a good, standard default pick.
PREFIX := /usr/local

# Check if the architecture is supported.
ifeq ($(filter $(ARCH),aarch64 loongarch64 riscv64 x86_64),)
    $(error Architecture $(ARCH) not supported)
endif

# User controllable C compiler command.
CC := cc

# User controllable C flags.
CFLAGS := -g -O2 -pipe

# User controllable C preprocessor flags. We set none by default.
CPPFLAGS :=

ifeq ($(ARCH),x86_64)
    # User controllable nasm flags.
    NASMFLAGS := -F dwarf -g
endif

# User controllable linker flags. We set none by default.
LDFLAGS :=

# Ensure the dependencies have been obtained.
ifneq ($(shell ( test '$(MAKECMDGOALS)' = clean || test '$(MAKECMDGOALS)' = distclean ); echo $$?),0)
    ifeq ($(shell ( ! test -d src/deps/freestnd-c-hdrs || ! test -f src/deps/cc-runtime.c || ! test -f src/deps/limine.h || ! test -d src/deps/flanterm ); echo $$?),0)
        $(error Please run the ./get-deps script first)
    endif
endif

# Check if CC is Clang.
override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep 'clang' >/dev/null 2>&1; echo $$?)

# Internal C flags that should not be changed by the user.
override CFLAGS += \
    -Wall \
    -Wextra \
    -std=gnu11 \
    -nostdinc \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections

ifeq ($(CC_IS_CLANG), 1)
    # Force Clang to use it's own linker instead of the host's one, since it
    # might be used for cross-compilation.
    override LDFLAGS += \
        -fuse-ld=lld
endif

# Internal C preprocessor flags that should not be changed by the user.
override CPPFLAGS := \
    -I src \
    -isystem src/deps/freestnd-c-hdrs \
    $(CPPFLAGS) \
    -DLIMINE_API_REVISION=3 \
    -MMD \
    -MP

# Architecture specific internal flags.
ifeq ($(ARCH),x86_64)
    ifeq ($(CC_IS_CLANG),1)
        override CC += \
            -target x86_64-unknown-none
    endif
    override CFLAGS += \
        -m64 \
        -march=x86-64 \
        -mno-80387 \
        -mno-mmx \
        -mno-sse \
        -mno-sse2 \
        -mno-red-zone \
        -mcmodel=kernel
    override LDFLAGS += \
        -Wl,-m,elf_x86_64
    override NASMFLAGS += \
        -f elf64
endif
ifeq ($(ARCH),aarch64)
    ifeq ($(CC_IS_CLANG),1)
        override CC += \
            -target aarch64-unknown-none
    endif
    override CFLAGS += \
        -mgeneral-regs-only
    override LDFLAGS += \
        -Wl,-m,aarch64elf
endif
ifeq ($(ARCH),riscv64)
    ifeq ($(CC_IS_CLANG),1)
        override CC += \
            -target riscv64-unknown-none
        override CFLAGS += \
            -march=rv64imac
    else
        override CFLAGS += \
            -march=rv64imac_zicsr_zifencei
    endif
    override CFLAGS += \
        -mabi=lp64 \
        -mno-relax
    override LDFLAGS += \
        -Wl,-m,elf64lriscv \
        -Wl,--no-relax
endif
ifeq ($(ARCH),loongarch64)
    ifeq ($(CC_IS_CLANG),1)
        override CC += \
            -target loongarch64-unknown-none
    endif
    override CFLAGS += \
        -march=loongarch64 \
        -mabi=lp64s
    override LDFLAGS += \
        -Wl,-m,elf64loongarch \
        -Wl,--no-relax
endif

# Internal linker flags that should not be changed by the user.
override LDFLAGS += \
    -Wl,--build-id=none \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -Wl,--gc-sections \
    -T linker-$(ARCH).ld

# Use "find" to glob all *.c, *.S, and *.asm files in the tree and obtain the
# object and header dependency file names.
override SRCFILES := $(shell cd src && find -L * -type f | LC_ALL=C sort)
override CFILES := $(filter %.c,$(SRCFILES))
override ASFILES := $(filter %.S,$(SRCFILES))
ifeq ($(ARCH),x86_64)
override NASMFILES := $(filter %.asm,$(SRCFILES))
endif
override OBJ := $(addprefix obj-$(ARCH)/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o))
ifeq ($(ARCH),x86_64)
override OBJ += $(addprefix obj-$(ARCH)/,$(NASMFILES:.asm=.asm.o))
endif
override HEADER_DEPS := $(addprefix obj-$(ARCH)/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

# Default target. This must come first, before header dependencies.
.PHONY: all
all: bin-$(ARCH)/$(OUTPUT)

# Include header dependencies.
-include $(HEADER_DEPS)

# Link rules for the final executable.
bin-$(ARCH)/$(OUTPUT): GNUmakefile linker-$(ARCH).ld $(OBJ)
	@echo "  LD  $@"
	@mkdir -p "$$(dirname $@)"
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

# Compilation rules for *.c files.
obj-$(ARCH)/%.c.o: src/%.c GNUmakefile
	@echo "  CC  $@"
	@mkdir -p "$$(dirname $@)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compilation rules for *.S files.
obj-$(ARCH)/%.S.o: src/%.S GNUmakefile
	@echo "  CC  $@"
	@mkdir -p "$$(dirname $@)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

ifeq ($(ARCH),x86_64)
# Compilation rules for *.asm (nasm) files.
obj-$(ARCH)/%.asm.o: src/%.asm GNUmakefile
	@echo "  NASM  $@"
	@mkdir -p "$$(dirname $@)"
	@nasm $(NASMFLAGS) $< -o $@
endif

# Remove object files and the final executable.
.PHONY: clean
clean:
	@rm -rf bin-$(ARCH) obj-$(ARCH)

# Remove everything built and generated including downloaded dependencies.
.PHONY: distclean
distclean:
	@rm -rf bin-* obj-* src/deps

# Install the final built executable to its final on-root location.
.PHONY: install
install: all
	@install -d "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)"
	@install -m 644 bin-$(ARCH)/$(OUTPUT) "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)/$(OUTPUT)-$(ARCH)"
	@echo "A new copy of Soaplin has been installed to $(DESTDIR)$(PREFIX)/share/$(OUTPUT)."

# Try to undo whatever the "install" target did.
.PHONY: uninstall
uninstall:
	@rm -f "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)/$(OUTPUT)-$(ARCH)"
	@-rmdir "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)"
	@echo "The copy of Soaplin at $(DESTDIR)$(PREFIX)/share/$(OUTPUT) has been deleted."
