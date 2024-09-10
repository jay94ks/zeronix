#include "except.h"
#include "idt.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/layout.h>

// --
extern void karch_except_00();
extern void karch_except_01();
extern void karch_except_02();
extern void karch_except_03();
extern void karch_except_04();
extern void karch_except_05();
extern void karch_except_06();
extern void karch_except_07();
extern void karch_except_08();
extern void karch_except_09();
extern void karch_except_10();
extern void karch_except_11();
extern void karch_except_12();
extern void karch_except_13();
extern void karch_except_14();
extern void karch_except_16();
extern void karch_except_17();
extern void karch_except_18();

#define MAX_EXCEPT_CB   19

// --
static const struct gatevec gv_except[] = {
    { karch_except_00,  0, PRIV_KERN }, // --> divide error
    { karch_except_01,  1, PRIV_KERN }, // --> debug vector, single step.
    { karch_except_02,  2, PRIV_KERN }, // --> nmi.
    { karch_except_03,  3, PRIV_USER }, // --> breakpoint.
    { karch_except_04,  4, PRIV_USER }, // --> overflow
    { karch_except_05,  5, PRIV_KERN }, // --> bounds
    { karch_except_06,  6, PRIV_KERN }, // --> inval opcode
    { karch_except_07,  7, PRIV_KERN }, // --> copr not avail.
    { karch_except_08,  8, PRIV_KERN }, // --> double fault.
    { karch_except_09,  9, PRIV_KERN }, // --> coproc seg overflow.
    { karch_except_10, 10, PRIV_KERN }, // --> invalid tss.
    { karch_except_11, 11, PRIV_KERN }, // --> segment not present.
    { karch_except_12, 12, PRIV_KERN }, // --> stack fault.
    { karch_except_13, 13, PRIV_KERN }, // --> general protection fault.
    { karch_except_14, 14, PRIV_KERN }, // --> page fault.

    { karch_except_16, 16, PRIV_KERN }, // --> coproc error.
    { karch_except_17, 17, PRIV_KERN }, // --> alignment check.
    { karch_except_18, 18, PRIV_KERN }, // --> machine check.

    GATEVEC_NULL
};

// --
karch_except_cb_t except_cb[MAX_EXCEPT_CB];
void* except_cb_data[MAX_EXCEPT_CB];

// --
void karch_init_except() {
    kmemset(except_cb, 0, sizeof(except_cb));
    kmemset(except_cb_data, 0, sizeof(except_cb_data));

    // --> map exception handler vectors.
    karch_load_idt_gatevec(gv_except, 0);
}

uint8_t karch_get_except(uint8_t n, karch_except_cb_t* cb, void** data) {
    if (n > MAX_EXCEPT_CB || n == 15) {
        return 0;
    }

    if (cb) {
        *cb = except_cb[n];
    }

    if (data) {
        *data = except_cb_data[n];
    }

    return 1;
}

uint8_t karch_set_except(uint8_t n, karch_except_cb_t cb, void* data) {
    if (n > MAX_EXCEPT_CB || n == 15) {
        return 0;
    }

    except_cb[n] = cb;
    except_cb_data[n] = data;

    return 1;
}

void karch_except(uint32_t n, uint32_t k, karch_except_frame_t* frame) {
    if (except_cb[n]) {
        except_t e;

        e.n = n;
        e.k = k;
        e.frame = frame;
        e.data = except_cb_data[n];

        except_cb[n](&e);
    }

    if (!k) {
        // --> switch to user if possible.
    }
}