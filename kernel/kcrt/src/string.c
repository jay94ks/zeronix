#include <zeronix/kcrt/string.h>

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