#include <zeronix/kstring.h>

void kmemset(void* dst, uint8_t val, size_t len) {
    uint32_t u32_val = 0;

    *(((uint8_t*)&u32_val) + 0) = val;
    *(((uint8_t*)&u32_val) + 1) = val;
    *(((uint8_t*)&u32_val) + 2) = val;
    *(((uint8_t*)&u32_val) + 3) = val;

    uint8_t* u8_ptr = (uint8_t*) dst;
    size_t i = 0;

    for (; i < len; i += sizeof(uint32_t)) {
        *((uint32_t*)(u8_ptr + i)) = u32_val;
    }

    for(; i < len; ++i) {
        *(u8_ptr + i) = val;
    }
}

void kmemcpy(void* dst, const void* src, size_t len) {
    uint32_t* u32_dst = (uint32_t*) dst;
    uint32_t* u32_src = (uint32_t*) src;

    while(len >= sizeof(uint32_t)) {
        *u32_dst++ = *u32_src++;
        len -= sizeof(uint32_t);
    }

    uint8_t* u8_dst = (uint8_t*) u32_dst;
    uint8_t* u8_src = (uint8_t*) u32_src;

    while(len) {
        *u8_dst++ = *u8_src++;
        len--;
    }
}

void kmemmove(void* dst, const void* src, size_t len) {
    if (!len) {
        return;
    }

    if (dst < src) {
        kmemcpy(dst, src, len);
        return;
    }
    
    uint32_t* u32_dst = (uint32_t*) ((uint8_t*)dst + len);
    uint32_t* u32_src = (uint32_t*) ((uint8_t*)src + len);
    while (len >= sizeof(uint32_t)) {
        *(--u32_dst) = *(--u32_src);
        len -= sizeof(uint32_t);
    }

    uint8_t* u8_dst = (uint8_t*) u32_dst;
    uint8_t* u8_src = (uint8_t*) u32_src;
    while(len) {
        *(--u8_dst) = *(--u8_src);
        len--;
    }
}

int32_t kmemcmp(const void* left, const void* right, size_t len) {
    int32_t* s32_left = (int32_t*) left;
    int32_t* s32_right = (int32_t*) right;
    int32_t dv = 0;

    while (len >= sizeof(int32_t)) {
        int32_t vl = *s32_left++;
        int32_t vr = *s32_right++;
        if ((dv = vl - vr) != 0) {
            break;
        }
        
        len -= sizeof(int32_t);
    }

    uint8_t* u8_left = (uint8_t*) s32_left;
    uint8_t* u8_right = (uint8_t*) s32_right;

    while (len) {
        int32_t vl = *u8_left++;
        int32_t vr = *u8_right++;
        if ((dv = vl - vr) != 0) {
            return dv;
        }

        len--;
    }

    return 0;
}

const void* kmemchr(const void* mem, uint8_t val, size_t len) {
    const uint8_t* u8_mem = (const uint8_t*) mem;

    while (len) {
        if (*u8_mem == val) {
            return u8_mem;
        }

        u8_mem++;
        len--;
    }

    return 0;
}

const void* kmemrchr(const void* mem, uint8_t val, size_t len) {
    if (!len) {
        return 0;
    }

    const uint8_t* u8_rev = (const uint8_t*) mem + (len - 1);
    while (len) {
        if (*u8_rev == val) {
            return u8_rev;
        }

        u8_rev--;
        len--;
    }

    return 0;
}

const void* kmemmem(const void* mem, size_t len, const void* sub, size_t sublen) {
    const uint8_t* u8_mem = (const uint8_t*) mem;
    if (len < sublen) {
        return 0;
    }

    for(size_t i = 0; i <= len - sublen; ++i) {
        if (kmemcmp(u8_mem + i, sub, sublen) == 0) {
            return u8_mem + i;
        }
    }

    return 0;
}

const void* kmemrmem(const void* mem, size_t len, const void* sub, size_t sublen) {
    const uint8_t* u8_mem = (const uint8_t*) mem;
    if (len < sublen) {
        return 0;
    }

    // end to `1`.
    for(size_t i = len - sublen; i > 0; --i) {
        if (kmemcmp(u8_mem + i, sub, sublen) == 0) {
            return u8_mem + i;
        }
    }

    // --> finally `begin`.
    if (kmemcmp(u8_mem, sub, sublen) == 0) {
        return u8_mem;
    }

    return 0;
}

size_t kstrlen(const char* str) {
    size_t n = 0;

    while (*str++) {
        n++;
    }

    return n;
}

/**
 * compare two string.
 */
int32_t kstrcmp(const char* left, const char* right) {
    char l, r;
    int32_t v;

    do {
        l = *left++;
        r = *right++;
        v = (int32_t)l - (int32_t)r;
    }

    while(l && r && !v);
    return v;
}

/**
 * compare two string.
 */
int32_t kstrncmp(const char* left, const char* right, size_t n) {
    char l, r;
    int32_t v;

    if (!n) {
        return 0;
    }

    do {
        l = *left++;
        r = *right++;
        n--;
        v = (int32_t)l - (int32_t)r;
    }

    while(l && r && !v && n);
    return v;
}

const char* kstrcpy(char* dst, const char* src) {
    while(*src) {
        *dst++ = *src++;
    }

    *dst = 0;
    return dst;
}

const char* kstrncpy(char* dst, const char* src, size_t n) {
    while(*src && n) {
        *dst++ = *src++;
        n--;
    }

    if (n) {
        *dst = 0;
    }

    return dst;
}

const char* kstrchr(const char* src, char ch) {
    while(*src) {
        if (*src == ch) {
            return src;
        }

        src++;
    }

    return 0;
}

const char* kstrnchr(const char* src, char ch, size_t n) {
    while(*src && n) {
        if (*src == ch) {
            return src;
        }

        src++;
        n--;
    }

    return 0;
}

const char* kstrrchr(const char* src, char ch) {
    const char* last = 0;

    while(*src) {
        if (*src == ch) {
            last = src;
        }

        src++;
    }

    return last;
}

const char* kstrrnchr(const char* src, char ch, size_t n) {
    const char* last = 0;

    while(*src && n) {
        if (*src == ch) {
            last = src;
        }

        src++;
        n--;
    }

    return last;
}

const char* kstrstr(const char* str, const char* sub) {
    size_t len = kstrlen(sub);
    while(*str) {
        if (!kstrncmp(str, sub, len)) {
            return str;
        }

        str++;
    }

    return 0;
}

const char* kstrrstr(const char* str, const char* sub) {
    size_t len = kstrlen(sub);
    const char* last = 0;

    while(*str) {
        if (!kstrncmp(str, sub, len)) {
            last = str;
        }

        str++;
    }

    return last;
}