/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#define ELF_BITS        32

typedef int             intptr_t;
typedef unsigned int    uintptr_t;

typedef enum {
    Page4K
} pagesize_t;

#include "x86_common.h"

#define PAGE_SIZE_DEFAULT       PAGE_SIZE_4K
#define PAGE_SIZE_KERNEL        PAGE_SIZE_4K
