#ifndef __ZERONIX_KCRT_STRING_H__
#define __ZERONIX_KCRT_STRING_H__

#include <zeronix/types.h>
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void kmemset(void* dst, uint8_t val, size_t len);
void kmemcpy(void* dst, const void* src, size_t len);
void kmemmove(void* dest, const void* src, size_t len);
int32_t kmemcmp(const void* left, const void* right, size_t len);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // __ZERONIX_KCRT_STRING_H__