CURRENT_PATH = $(shell pwd)
TARGET_MODULE = $(CURRENT_PATH)/main.ko
TARGET_ARCH = x86

KERNEL_IMAGE = $(CURRENT_PATH)/kernel.elf

include $(CURRENT_PATH)/arch/$(TARGET_ARCH)/toolchain.inc

SRCS_CC		= $(shell find ./src/ -name "*.c" | sort)
SRCS_ASM	= $(shell find ./src/ -name "*.asm" | sort)
SRCS_CXX	= $(shell find ./src/ -name "*.cpp" | sort)

OBJS  = $(SRCS_ASM:.asm=.asm.o)
OBJS += $(SRCS_CC:.c=.c.o)
OBJS += $(SRCS_CXX:.cpp=.cpp.o)

KCRT_DIR = $(CURRENT_PATH)/kcrt
KERN_KO = $(ARCH_DIR)/arch.ko $(KCRT_DIR)/kcrt.ko

MAKEOPT = TARGET_ARCH=$(TARGET_ARCH)

default:
	$(MAKE) $(KERNEL_IMAGE)

debug: 
	$(MAKE) clean
	$(MAKE) debug-2nd
	
debug-2nd: $(KERNEL_IMAGE)
	qemu-system-i386 -m 256M -smp sockets=1,cores=2,threads=2 -kernel $(KERNEL_IMAGE) -s -S

run: $(KERNEL_IMAGE)
	qemu-system-i386 -m 256M -smp sockets=1,cores=2,threads=2 -kernel $(KERNEL_IMAGE)

clean:
	@find . -type f -name "*.o" -exec rm {} \;
	@find . -type f -name "*.o.tmp" -exec rm {} \;
	@if [ -f $(TARGET_MODULE) ]; then rm $(TARGET_MODULE); fi
	$(MAKE) $(MAKEOPT) -C $(ARCH_DIR)/ clean
	$(MAKE) $(MAKEOPT) -C $(KCRT_DIR)/ clean

$(TARGET_MODULE): $(OBJS)
	$(LD) -r $(shell find . -name "*.o" | sort) -o $(TARGET_MODULE)

$(KERNEL_IMAGE): $(TARGET_MODULE) $(KERN_KO)
	$(LD) -T $(LDSCRIPT) $(KERN_KO) $(TARGET_MODULE) -o $(KERNEL_IMAGE)
	$(OBJDUMP) -D $(KERNEL_IMAGE) > $(KERNEL_IMAGE).dump

$(ARCH_DIR)/arch.ko:
	$(MAKE) $(MAKEOPT) -C $(ARCH_DIR)/

$(KCRT_DIR)/kcrt.ko:
	$(MAKE) $(MAKEOPT) -C $(KCRT_DIR)/
	
%.c.o: %.c
	echo $(ARCH_DIR)
	$(CC) -c $< $(CFLAGS) -o $@

%.asm.o: %.asm
	$(NASM) $(NASMFLAGS) $< -o $@

%.cpp.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@