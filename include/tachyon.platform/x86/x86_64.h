/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#define ELF_BITS            64

typedef long long           intptr_t;
typedef unsigned long long  uintptr_t;

#include "asm/x86_64.h"
#include "x86_common.h"

#define PAGE_SIZE_DEFAULT       PAGE_SIZE_4K
#define PAGE_SIZE_LARGE         PAGE_SIZE_2M

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

    uint32_t    pad0;

    uintptr_t   r8;
    uintptr_t   r9;
    uintptr_t   r10;
    uintptr_t   r11;
    uintptr_t   r12;
    uintptr_t   r13;
    uintptr_t   r14;
    uintptr_t   r15;
} cpustate_t;

