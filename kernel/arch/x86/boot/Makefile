CURRENT_PATH = $(shell pwd)
TARGET_MODULE = $(CURRENT_PATH)/boot.ko

include $(CURRENT_PATH)/../toolchain.inc

SRCS_CC		= $(shell find ./src/ -name "*.c" | sort)
SRCS_ASM	= $(shell find ./src/ -name "*.asm" | sort)
SRCS_CXX	= $(shell find ./src/ -name "*.cpp" | sort)

OBJS  = $(SRCS_ASM:.asm=.asm.o)
OBJS += $(SRCS_CC:.c=.c.o)
OBJS += $(SRCS_CXX:.cpp=.cpp.o)

default:
	$(MAKE) $(TARGET_MODULE)

clean:
	@find . -type f -name "*.o" -exec rm {} \;
	@find . -type f -name "*.o.tmp" -exec rm {} \;
	@if [ -f $(TARGET_MODULE) ]; then rm $(TARGET_MODULE); fi

$(TARGET_MODULE): $(OBJS)
	$(LD) -r $(shell find . -name "*.o" | sort) -o $(TARGET_MODULE)
	
%.c.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@.tmp
	$(OBJCPY) --prefix-sections=.unpaged $@.tmp $@
	rm -rf $@.tmp

%.asm.o: %.asm
	$(NASM) $(NASMFLAGS) $< -o $@.tmp
	$(OBJCPY) --prefix-sections=.unpaged $@.tmp $@
	rm -rf $@.tmp

%.cpp.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@.tmp
	$(OBJCPY) --prefix-sections=.unpaged $@.tmp $@
	rm -rf $@.tmp