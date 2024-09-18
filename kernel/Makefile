CURRENT_PATH = $(shell pwd)
TARGET_ARCH=x86

MODS=$(shell find ./arch/$(TARGET_ARCH)/ -name Makefile | sort)
LIBS=$(shell find ./lib/ -name Makefile | sort)
KERNEL=$(shell find ./mods/ -name Makefile | sort)

MODS += $(LIBS)
MODS += $(KERNEL)

TEMP=$(subst /Makefile,,${MODS})
KO_FILES=$(patsubst %, %.ko, $(TEMP))

KERNEL_IMAGE = $(CURRENT_PATH)/kernel.elf
ARCH_DIR = $(CURRENT_PATH)/arch/$(TARGET_ARCH)
include $(ARCH_DIR)/toolchain.mk

.SUFFIXES: .c .cpp .asm .ko

default: $(KERNEL_IMAGE) $(KERNEL_IMAGE).dump

debug: 
	$(MAKE) clean
	$(MAKE) debug-2nd
	
debug-2nd: $(KERNEL_IMAGE) $(KERNEL_IMAGE).dump
	@if [ -f ./qemu-debug.log ]; then rm ./qemu-debug.log; fi
	qemu-system-i386 -D ./qemu-debug.log -d cpu_reset,in_asm -m 256M -smp sockets=1,cores=2,threads=2 -kernel $(KERNEL_IMAGE) -s -S
	#qemu-system-x86_64 -D ./qemu-debug.log -d cpu_reset,in_asm -m 256M -smp 4 -kernel $(KERNEL_IMAGE) -s -S

run: $(KERNEL_IMAGE)
	@if [ -f ./qemu-debug.log ]; then rm ./qemu-debug.log; fi
	qemu-system-i386 -D ./qemu-debug.log -d cpu_reset,in_asm -m 256M -smp sockets=1,cores=2,threads=2 -kernel $(KERNEL_IMAGE)
	#qemu-system-x86_64 -D ./qemu-debug.log -d cpu_reset,in_asm -m 256M -smp 4 -kernel $(KERNEL_IMAGE)

clean:
	@find . -type f -name "*.o" -exec rm {} \;
	@find . -type f -name "*.o.tmp" -exec rm {} \;
	@find . -type f -name "*.ko" -exec rm {} \;

%.ko: %
	$(MAKE) ARCH_DIR=$(ARCH_DIR) -C $<

$(KERNEL_IMAGE): $(KO_FILES)
	$(LD) -T $(LDSCRIPT) $(KO_FILES) -o $(KERNEL_IMAGE)

$(KERNEL_IMAGE).dump: $(KERNEL_IMAGE)
	$(OBJDUMP) -M intel -D $(KERNEL_IMAGE) > $(KERNEL_IMAGE).dump