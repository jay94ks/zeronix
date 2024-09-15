#ifndef __ZERONIX_ARCH_X86_LAYOUT_H__
#define __ZERONIX_ARCH_X86_LAYOUT_H__

/**
 * do not include this file except arch specific code space.
 * this can cause compiler-dizzy due to macro definitions. 
 */

#define PRIV_KERN   0
#define PRIV_USER   3
#define PRIV_MASK   0x03

// --
#define MAX_IDT 256
#define MAX_CPU 16

enum {
    GDT_NULL = 0,
    GDT_KERN_CS,
    GDT_KERN_DS,
    GDT_USER_CS,
    GDT_USER_DS,
    GDT_LDT,
    GDT_TSS,

    /* TODO: tss per cpu. */

    GDT_MAX = GDT_TSS + MAX_CPU
};

/* Index for TSS. */
#define GDT_TSS_INDEX(x)    (GDT_TSS + (x))

// --> segment selector.
#define SEG_SEL(x)          ((x) * 8)
#define SEG_TSS(x)          (SEG_SEL(GDT_TSS) + (x))

// --
#define BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#define GRANULARITY_SHIFT   16  /* shift for limit --> granularity */
#define OFFSET_HIGH_SHIFT   16  /* shift for (gate) offset --> offset_high */
#define PAGE_GRAN_SHIFT     12  /* extra shift for page granular limits */

// --
#define GRANULAR  	  0x80	/* set for 4K granularilty */
#define DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#define BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#define AVL        	  0x10	/* 0 for available */
#define LIMIT_HIGH   	  0x0F	/* mask for high bits of limit */

/* Selector bits. */
#define TI                0x04	/* table indicator */
#define RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#define BASE_MIDDLE_SHIFT   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#define PRESENT           0x80	/* set for descriptor present */
#define DPL               0x60	/* descriptor privilege level mask */
#define DPL_SHIFT            5
#define SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#define EXECUTABLE        0x08	/* set for executable segment */
#define CONFORMING        0x04	/* set for conforming segment if executable */
#define EXPAND_DOWN       0x04	/* set for expand-down segment if !executable*/
#define READABLE          0x02	/* set for readable segment if executable */
#define WRITEABLE         0x02	/* set for writeable segment if !executable */
#define TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#define ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#define AVL_286_TSS          1	/* available 286 TSS */
#define LDT                  2	/* local descriptor table */
#define BUSY_286_TSS         3	/* set transparently to the software */
#define CALL_286_GATE        4	/* not used */
#define TASK_GATE            5	/* only used by debugger */
#define INT_286_GATE         6	/* interrupt gate, used for all vectors */
#define TRAP_286_GATE        7	/* not used */

#define INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#define TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)

/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#define PAGE_FAULT_VECTOR   14
#define COPROC_ERR_VECTOR   16	/* coprocessor error */
#define ALIGNMENT_CHECK_VECTOR	17
#define MACHINE_CHECK_VECTOR	18
#define SIMD_EXCEPTION_VECTOR	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#define DESC_GRANULARITY     6	/* to granularity byte */
#define DESC_BASE_HIGH       7	/* to base_high */

/* Type-byte bits. */
#define DESC_386_BIT  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 

/* Granularity byte. */

/* Program stack words and masks. */
#define INIT_PSW      0x0200    /* initial psw */
#define INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#define TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#define SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#define IF_MASK 0x00000200
#define IOPL_MASK 0x003000

#define INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#define INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#define INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#define AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#define AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#define AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

/* fpu context should be saved in 16-byte aligned memory */
#define FPUALIGN		16

/* Poweroff 16-bit code address */
#define BIOS_POWEROFF_ENTRY 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#define PG_ALLOCATEME ((phys_bytes)-1)

/* MSRs */
#define INTEL_MSR_PERFMON_CRT0         0xc1
#define INTEL_MSR_SYSENTER_CS         0x174
#define INTEL_MSR_SYSENTER_ESP        0x175
#define INTEL_MSR_SYSENTER_EIP        0x176
#define INTEL_MSR_PERFMON_SEL0        0x186

#define INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#define AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#define AMD_MSR_EFER		0xC0000080	/* extended features msr */
#define AMD_MSR_STAR		0xC0000081	/* SYSCALL params msr */

#endif // __ZERONIX_ARCH_X86_LAYOUT_H__