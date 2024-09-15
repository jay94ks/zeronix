#ifndef __INC_ZERONIX_KSTRING_H__
#define __INC_ZERONIX_KSTRING_H__

/**
 * This functions are implemented by `arch/{target}/klib/src/string.c`
*/
#include <zeronix/types.h>
#ifdef __cplusplus
extern "C" {
#endif

void kmemset(void* dst, uint8_t val, size_t len);
void kmemcpy(void* dst, const void* src, size_t len);
void kmemmove(void* dest, const void* src, size_t len);
int32_t kmemcmp(const void* left, const void* right, size_t len);

#ifdef __cplusplus
}
#endif

#endif