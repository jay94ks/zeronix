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

KERN_KO = $(ARCH_DIR)/arch.ko

default:
	$(MAKE) $(KERNEL_IMAGE)

debug: $(KERNEL_IMAGE)
	qemu-system-i386 -kernel $(KERNEL_IMAGE)

clean:
	@find . -type f -name "*.o" -exec rm {} \;
	@find . -type f -name "*.o.tmp" -exec rm {} \;
	@if [ -f $(TARGET_MODULE) ]; then rm $(TARGET_MODULE); fi
	$(MAKE) -C $(ARCH_DIR)/ clean

$(TARGET_MODULE): $(OBJS)
	$(LD) -r $(shell find . -name "*.o" | sort) -o $(TARGET_MODULE)

$(KERNEL_IMAGE): $(TARGET_MODULE) $(KERN_KO)
	$(LD) -T $(LDSCRIPT) $(KERN_KO) $(TARGET_MODULE) -o $(KERNEL_IMAGE)

$(ARCH_DIR)/arch.ko:
	$(MAKE) -C $(ARCH_DIR)/
	
%.c.o: %.c
	echo $(ARCH_DIR)
	$(CC) -c $< $(CFLAGS) -o $@

%.asm.o: %.asm
	$(NASM) $(NASMFLAGS) $< -o $@

%.cpp.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@