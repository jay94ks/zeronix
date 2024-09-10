MAKE  		= /usr/bin/make
NASM  		= /usr/bin/nasm
CC    		= /opt/cross/bin/i686-elf-gcc
CXX   		= /opt/cross/bin/i686-elf-g++
LD    		= /opt/cross/bin/i686-elf-ld
STRIP 		= /opt/cross/bin/i686-elf-strip
OBJCPY		= /opt/cross/bin/i686-elf-objcopy
OBJDUMP     = /opt/cross/bin/i686-elf-objdump

TARGET_ARCH = x86
ARCH_DIR    = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
INC_DIR     = $(ARCH_DIR)/../../include

CDEFS       = -D__ARCH_X86__=1

# CFLAGS  	= -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I $(INC_DIR) $(CDEFS)
# CXXFLAGS	= -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -I $(INC_DIR) $(CDEFS)

CFLAGS  	= -std=gnu99 -ffreestanding -g -Wall -Wextra -I $(INC_DIR) $(CDEFS)
CXXFLAGS	= -ffreestanding -g -Wall -Wextra -fno-exceptions -fno-rtti -I $(INC_DIR) $(CDEFS)
NASMFLAGS	= -felf32

LDSCRIPT	= $(ARCH_DIR)/link.ld

.SUFFIXES: .c .cpp .asm