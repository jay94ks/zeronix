CURRENT_PATH = $(shell pwd)
MODULE_NAME = $(basename $(CURRENT_PATH))
TARGET_MODULE = $(dirname $(CURRENT_PATH))$(MODULE_NAME).ko

INC_DIR = $(CURRENT_PATH)/inc
include $(ARCH_DIR)/toolchain.mk

SRCS_CC		= $(shell find ./src/ -name "*.c" | sort)
SRCS_ASM	= $(shell find ./src/ -name "*.asm" | sort)
SRCS_CXX	= $(shell find ./src/ -name "*.cpp" | sort)

OBJS  = $(SRCS_ASM:.asm=.asm.o)
OBJS += $(SRCS_CC:.c=.c.o)
OBJS += $(SRCS_CXX:.cpp=.cpp.o)

default:
	$(MAKE) $(TARGET_MODULE)

clean:
	@if [ -f $(TARGET_MODULE) ]; then rm $(TARGET_MODULE); fi

$(TARGET_MODULE): $(OBJS)
	$(LD) -r $(shell find . -name "*.o" | sort) -o $(TARGET_MODULE)
	
%.c.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

%.asm.o: %.asm
	$(NASM) $(NASMFLAGS) $< -o $@

%.cpp.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@