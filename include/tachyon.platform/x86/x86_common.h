/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include "asm/x86_common.h"

/* 
 * .--------------------------------.
 * | Low-level console related      |
 * '--------------------------------' 
 */

#define VRAM_LOCATION   0xB8000
#define KCON_WIDTH      80
#define KCON_HEIGHT     25
#define VRAM_SIZE       (KCON_WIDTH*KCON_HEIGHT*2)

/* 
 * .--------------------------------.
 * | C types for x86 and x86_64     |
 * '--------------------------------' 
 */

typedef signed      char        int8_t;
typedef signed      short       int16_t;
typedef signed      int         int32_t;
typedef signed      long long   int64_t;
typedef intptr_t                ssize_t;

typedef unsigned    char        uint8_t;
typedef unsigned    short       uint16_t;
typedef unsigned    int         uint32_t;
typedef unsigned    long long   uint64_t;
typedef uintptr_t               size_t;

typedef intptr_t    intmax_t;
typedef uintptr_t   uintmax_t;
typedef intmax_t    off_t;

typedef uintmax_t   pid_t;
typedef pid_t       tid_t;

typedef uintptr_t   phys_addr_t;
typedef phys_addr_t vspace_t;

typedef struct {
    uintptr_t   interrupt;
    uintptr_t   code;
    uintptr_t   ip;
    uintptr_t   cs;
    uintptr_t   flags;
    uintptr_t   sp;
    uintptr_t   ss;
} interrupt_t;

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * .--------------------------------.
 * | extern C functions             |
 * '--------------------------------' 
 */


#ifdef __cplusplus
} /* extern "C" */
#endif

