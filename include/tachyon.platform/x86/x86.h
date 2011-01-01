/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#define ELF_BITS        32

typedef int             intptr_t;
typedef unsigned int    uintptr_t;

#include "asm/x86.h"
#include "x86_common.h"

#define PAGE_SIZE_DEFAULT       PAGE_SIZE_4K
#define PAGE_SIZE_LARGE         PAGE_SIZE_4K

typedef struct {
    uintptr_t   ip;
    uintptr_t   flags;

    uintptr_t   ax;
    uintptr_t   cx;
    uintptr_t   dx;
    uintptr_t   bx;
    uintptr_t   sp;
    uintptr_t   bp;
    uintptr_t   si;
    uintptr_t   di;

    uint16_t    es;
    uint16_t    cs;
    uint16_t    ss;
    uint16_t    ds;
    uint16_t    fs;
    uint16_t    gs;
} PACKED cpustate_t;

