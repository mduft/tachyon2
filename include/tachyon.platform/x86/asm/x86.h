/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include "x86_common.h"

/* 
 * .--------------------------------.
 * | Kernel location in memory      |
 * '--------------------------------' 
 */

#define CORE_LMA_X86    0x00100000
#define CORE_VMA_X86    0xC0000000

#define CORE_HEAP_START 0xC1000000
#define CORE_HEAP_END   0xE0000000

#define LAPIC_VIRTUAL   0xE0000000
#define LAPIC_PHYSICAL  0xFFE00000
