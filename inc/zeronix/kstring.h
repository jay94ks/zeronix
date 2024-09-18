#ifndef __INC_ZERONIX_KSTRING_H__
#define __INC_ZERONIX_KSTRING_H__

/**
 * This functions are implemented by `arch/{target}/klib/src/string.c`
*/
#include <zeronix/types.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * fill `val` to `dst` ~ `len` memory.
 */
void kmemset(void* dst, uint8_t val, size_t len);

/**
 * copy `src` to `dst` memory.
 */
void kmemcpy(void* dst, const void* src, size_t len);

/**
 * move `src` to `dst` memory.
 */
void kmemmove(void* dst, const void* src, size_t len);

/**
 * compare two memory.
 */
int32_t kmemcmp(const void* left, const void* right, size_t len);

/**
 * find `val` in memory.
 */
const void* kmemchr(const void* mem, uint8_t val, size_t len);

/**
 * find `val` in memory.
 */
const void* kmemrchr(const void* mem, uint8_t val, size_t len);

/**
 * find `sub` in memory.
 */
const void* kmemmem(const void* mem, size_t len, const void* sub, size_t sublen);

/**
 * find `sub` in memory.
 */
const void* kmemrmem(const void* mem, size_t len, const void* sub, size_t sublen);

/**
 * compute length of string.
 */
size_t kstrlen(const char* str);

/**
 * compare two string.
 */
int32_t kstrcmp(const char* left, const char* right);

/**
 * compare two string with max `n`.
 */
int32_t kstrncmp(const char* left, const char* right, size_t n);

/**
 * copy `src` string to `dst`, and 
 * this does not respect NUL termination of `dst` string.
 * and returns the pointer that has NUL termination.
 */
const char* kstrcpy(char* dst, const char* src);

/**
 * copy `src` string to `dst`, and 
 * this does not respect NUL termination of `dst` string.
 * and returns the pointer that has NUL termination.
 */
const char* kstrncpy(char* dst, const char* src, size_t n);

/**
 * find a character in string.
 */
const char* kstrchr(const char* src, char ch);

/**
 * find a character in string in reverse order.
 */
const char* kstrnchr(const char* src, char ch, size_t n);

/**
 * find a character in string.
 */
const char* kstrrchr(const char* src, char ch);

/**
 * find a character in string in reverse order.
 */
const char* kstrrnchr(const char* src, char ch, size_t n);

/**
 * find a sub string in string.
 */
const char* kstrstr(const char* str, const char* sub);

/**
 * find a sub string in string in reverse order.
 */
const char* kstrrstr(const char* str, const char* sub);


#ifdef __cplusplus
}
#endif

#endif