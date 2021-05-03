#ifndef INCLUDES_BASE_ASM_TYPES_H_
#define INCLUDES_BASE_ASM_TYPES_H_

typedef uint16_t umode_t;

/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */

typedef char __sc;
typedef unsigned char __uc;

typedef int8_t __s8;
typedef uint8_t __u8;

typedef int16_t __s16;
typedef uint16_t __u16;

typedef int32_t __s32;
typedef uint32_t __u32;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef int64_t __s64;
typedef uint64_t __u64;
#endif

/*
 * These aren't exported outside the kernel to avoid name space clashes
 */
#ifdef __KERNEL__

#include <linux/config.h>

#define BITS_PER_LONG 32

/* DMA addresses come in generic and 64-bit flavours.  */

#ifdef CONFIG_HIGHMEM
typedef u64 dma_addr_t;
#else
typedef u32 dma_addr_t;
#endif
typedef u64 dma64_addr_t;

#endif  // __KERNEL__

#endif  // INCLUDES_BASE_ASM_TYPES_H_
