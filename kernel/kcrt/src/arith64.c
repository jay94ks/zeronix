#include <zeronix/types.h>

typedef union
{
    uint64_t u64;
    int64_t s64;
    struct
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        uint32_t hi; uint32_t lo;
#else
        uint32_t lo; uint32_t hi;
#endif
    } u32;
    struct
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        int32_t hi; int32_t lo;
#else
        int32_t lo; int32_t hi;
#endif
    } s32;
} arith64_word;

// extract hi and lo 32-bit words from 64-bit value
#define arith64_hi(n) (arith64_word){.u64=n}.u32.hi
#define arith64_lo(n) (arith64_word){.u64=n}.u32.lo

// Negate a if b is negative, via invert and increment.
#define arith64_neg(a, b) (((a) ^ ((((int64_t)(b)) >= 0) - 1)) + (((int64_t)(b)) < 0))
#define arith64_abs(a) arith64_neg(a, a)

// Return the absolute value of a.
// Note LLINT_MIN cannot be negated.
int64_t __absvdi2(int64_t a)
{
    return arith64_abs(a);
}

// Return the result of shifting a left by b bits.
int64_t __ashldi3(int64_t a, int b)
{
    arith64_word w = {.s64 = a};

    b &= 63;

    if (b >= 32)
    {
        w.u32.hi = w.u32.lo << (b - 32);
        w.u32.lo = 0;
    } else if (b)
    {
        w.u32.hi = (w.u32.lo >> (32 - b)) | (w.u32.hi << b);
        w.u32.lo <<= b;
    }
    return w.s64;
}

// Return the result of arithmetically shifting a right by b bits.
int64_t __ashrdi3(int64_t a, int b)
{
    arith64_word w = {.s64 = a};

    b &= 63;

    if (b >= 32)
    {
        w.s32.lo = w.s32.hi >> (b - 32);
        w.s32.hi >>= 31; // 0xFFFFFFFF or 0
    } else if (b)
    {
        w.u32.lo = (w.u32.hi << (32 - b)) | (w.u32.lo >> b);
        w.s32.hi >>= b;
    }
    return w.s64;
}

// These functions return the number of leading 0-bits in a, starting at the
// most significant bit position. If a is zero, the result is undefined.
int __clzsi2(uint32_t a)
{
    int b, n = 0;
    b = !(a & 0xffff0000) << 4; n += b; a <<= b;
    b = !(a & 0xff000000) << 3; n += b; a <<= b;
    b = !(a & 0xf0000000) << 2; n += b; a <<= b;
    b = !(a & 0xc0000000) << 1; n += b; a <<= b;
    return n + !(a & 0x80000000);
}

int __clzdi2(uint64_t a)
{
    int b, n = 0;
    b = !(a & 0xffffffff00000000ULL) << 5; n += b; a <<= b;
    b = !(a & 0xffff000000000000ULL) << 4; n += b; a <<= b;
    b = !(a & 0xff00000000000000ULL) << 3; n += b; a <<= b;
    b = !(a & 0xf000000000000000ULL) << 2; n += b; a <<= b;
    b = !(a & 0xc000000000000000ULL) << 1; n += b; a <<= b;
    return n + !(a & 0x8000000000000000ULL);
}

// These functions return the number of trailing 0-bits in a, starting at the
// least significant bit position. If a is zero, the result is undefined.
int __ctzsi2(uint32_t a)
{
    int b, n = 0;
    b = !(a & 0x0000ffff) << 4; n += b; a >>= b;
    b = !(a & 0x000000ff) << 3; n += b; a >>= b;
    b = !(a & 0x0000000f) << 2; n += b; a >>= b;
    b = !(a & 0x00000003) << 1; n += b; a >>= b;
    return n + !(a & 0x00000001);
}

int __ctzdi2(uint64_t a)
{
    int b, n = 0;
    b = !(a & 0x00000000ffffffffULL) << 5; n += b; a >>= b;
    b = !(a & 0x000000000000ffffULL) << 4; n += b; a >>= b;
    b = !(a & 0x00000000000000ffULL) << 3; n += b; a >>= b;
    b = !(a & 0x000000000000000fULL) << 2; n += b; a >>= b;
    b = !(a & 0x0000000000000003ULL) << 1; n += b; a >>= b;
    return n + !(a & 0x0000000000000001ULL);
}

// Calculate both the quotient and remainder of the unsigned division of a by
// b. The return value is the quotient, and the remainder is placed in variable
// pointed to by c (if it's not NULL).
uint64_t __divmoddi4(uint64_t a, uint64_t b, uint64_t *c)
{
    if (b > a)                                  // divisor > numerator?
    {
        if (c) *c = a;                          // remainder = numerator
        return 0;                               // quotient = 0
    }
    if (!arith64_hi(b))                         // divisor is 32-bit
    {
        if (b == 0)                             // divide by 0
        {
            volatile char x = 0; x = 1 / x;     // force an exception
        }
        if (b == 1)                             // divide by 1
        {
            if (c) *c = 0;                      // remainder = 0
            return a;                           // quotient = numerator
        }
        if (!arith64_hi(a))                     // numerator is also 32-bit
        {
            if (c)                              // use generic 32-bit operators
                *c = arith64_lo(a) % arith64_lo(b);
            return arith64_lo(a) / arith64_lo(b);
        }
    }

    // let's do long division
    char bits = __clzdi2(b) - __clzdi2(a) + 1;  // number of bits to iterate (a and b are non-zero)
    uint64_t rem = a >> bits;                // init remainder
    a <<= 64 - bits;                            // shift numerator to the high bit
    uint64_t wrap = 0;                       // start with wrap = 0
    while (bits-- > 0)                          // for each bit
    {
        rem = (rem << 1) | (a >> 63);           // shift numerator MSB to remainder LSB
        a = (a << 1) | (wrap & 1);              // shift out the numerator, shift in wrap
        wrap = ((int64_t)(b - rem - 1) >> 63);  // wrap = (b > rem) ? 0 : 0xffffffffffffffff (via sign extension)
        rem -= b & wrap;                        // if (wrap) rem -= b
    }
    if (c) *c = rem;                            // maybe set remainder
    return (a << 1) | (wrap & 1);               // return the quotient
}

int64_t __divdi3(int64_t a, int64_t b)
{
    uint64_t q = __divmoddi4(arith64_abs(a), arith64_abs(b), (void *)0);
    return arith64_neg(q, a^b);
}

int __ffsdi2(uint64_t a)
{
    return a ? __ctzdi2(a) + 1 : 0;
}

uint64_t __lshrdi3(uint64_t a, int b)
{
    arith64_word w = {.u64 = a};

    b &= 63;

    if (b >= 32)
    {
        w.u32.lo = w.u32.hi >> (b - 32);
        w.u32.hi = 0;
    } else if (b)
    {
        w.u32.lo = (w.u32.hi << (32 - b)) | (w.u32.lo >> b);
        w.u32.hi >>= b;
    }
    return w.u64;
}

int64_t __moddi3(int64_t a, int64_t b)
{
    uint64_t r;
    __divmoddi4(arith64_abs(a), arith64_abs(b), &r);
    return arith64_neg(r, a);
}

int __popcountsi2(uint32_t a)
{
    a = a - ((a >> 1) & 0x55555555);
    a = ((a >> 2) & 0x33333333) + (a & 0x33333333);
    a = (a + (a >> 4)) & 0x0F0F0F0F;
    a = (a + (a >> 16));
    
    return (a + (a >> 8)) & 63;
}

int __popcountdi2(uint64_t a)
{
    a = a - ((a >> 1) & 0x5555555555555555ULL);
    a = ((a >> 2) & 0x3333333333333333ULL) + (a & 0x3333333333333333ULL);
    a = (a + (a >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    a = (a + (a >> 32));
    a = (a + (a >> 16));

    return (a + (a >> 8)) & 127;
}

uint64_t __udivdi3(uint64_t a, uint64_t b)
{
    return __divmoddi4(a, b, (void *)0);
}

uint64_t __umoddi3(uint64_t a, uint64_t b)
{
    uint64_t r;
    __divmoddi4(a, b, &r);
    return r;
}