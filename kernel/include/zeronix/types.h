#ifndef __ZERNOIX_TYPES_H__
#define __ZERNOIX_TYPES_H__

#ifdef _MSC_VER
#define __attribute__(...)
#endif

#define __aligned(n)    __attribute__((aligned(n)))
#define __packed        __attribute__((packed))


/**
 * standard integer types.
 */
typedef char                int8_t;
typedef short               int16_t;
typedef int                 int32_t;
typedef long long           int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

/**
 * size type.
 */
typedef uint32_t            size_t;



#endif 