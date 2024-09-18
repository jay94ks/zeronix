#ifndef __KERNEL_ARCH_X86_INC_X86_I686_H__
#define __KERNEL_ARCH_X86_INC_X86_I686_H__

/* CR0 bits */
#define I686_CR0_PE		0x00000001	/* Protected mode  */
#define I686_CR0_MP		0x00000002	/* Monitor Coprocessor  */
#define I686_CR0_EM		0x00000004	/* Emulate  */
#define I686_CR0_TS		0x00000008	/* Task Switched  */
#define I686_CR0_ET		0x00000010	/* Extension Type  */
#define I686_CR0_WP		0x00010000	/* Enable paging */
#define I686_CR0_PG		0x80000000	/* Enable paging */

/* some CR4 bits */
#define I686_CR4_VME		0x00000001	/* Virtual 8086 */
#define I686_CR4_PVI		0x00000002	/* Virtual ints */
#define I686_CR4_TSD		0x00000004	/* RDTSC privileged */
#define I686_CR4_DE		0x00000008	/* Debugging extensions */
#define I686_CR4_PSE		0x00000010	/* Page size extensions */
#define I686_CR4_PAE		0x00000020	/* Physical addr extens. */
#define I686_CR4_MCE		0x00000040	/* Machine check enable */
#define I686_CR4_PGE		0x00000080	/* Global page flag enable */

/* CPUID flags */
#define CPUID1_EDX_FPU		(1L)		/* FPU presence */
#define CPUID1_EDX_PSE 		(1L <<  3)	/* Page Size Extension */
#define CPUID1_EDX_SYSENTER	(1L << 11)	/* Intel SYSENTER */
#define CPUID1_EDX_PAE 		(1L << 6)	/* Physical Address Extension */
#define CPUID1_EDX_PGE 		(1L << 13)	/* Page Global (bit) Enable */
#define CPUID1_EDX_APIC_ON_CHIP (1L << 9)	/* APIC is present on the chip */
#define CPUID1_EDX_TSC		(1L << 4)	/* Timestamp counter present */
#define CPUID1_EDX_HTT		(1L << 28)	/* Supports HTT */
#define CPUID1_EDX_FXSR		(1L << 24)
#define CPUID1_EDX_SSE		(1L << 25)
#define CPUID1_EDX_SSE2		(1L << 26)
#define CPUID1_ECX_SSE3		(1L)
#define CPUID1_ECX_SSSE3	(1L << 9)
#define CPUID1_ECX_SSE4_1	(1L << 19)
#define CPUID1_ECX_SSE4_2	(1L << 20)

#define CPUID_EF_EDX_SYSENTER	(1L << 11)	/* Intel SYSENTER */

#define CPUID1_EDX_SSE_FULL (CPUID1_EDX_FXSR | CPUID1_EDX_SSE | CPUID1_EDX_SSE2)
#define CPUID1_ECX_SSE_FULL (CPUID1_ECX_SSE3 | CPUID1_ECX_SSSE3 | CPUID1_ECX_SSE4_1 | CPUID1_ECX_SSE4_2)

/* Old BIOS areas. */

#define BIOS_OLD_BDA_START  0x00000000
#define BIOS_OLD_BDA_END    (0x000004ff + 1)

#define BIOS_EBDA_START     0x00080000
#define BIOS_EBDA_END       (0x0009ffff + 1)

#define BIOS_VIDEO_START    0x000a0000
#define BIOS_VIDEO_END      (0x000c7fff + 1)

#define BIOS_EXPAN_START    0x000c8000
#define BIOS_EXPAN_END      (0x000effff + 1)

#define BIOS_BOARD_START    0x000f0000
#define BIOS_BOARD_END      (0x000fffff + 1)

#endif