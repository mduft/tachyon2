/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include "x86_common.h"

/* 
 * .--------------------------------.
 * | Kernel location in memory      |
 * '--------------------------------' 
 */

#define CORE_LMA_X86_64     0x0000000000100000
#define CORE_VMA_X86_64     0xFFFFFFFF80000000

#define CORE_HEAP_START     0xFFFFFFFF81000000
#define CORE_HEAP_END       0xFFFFFFFFA0000000

#define APIC_VIRTUAL        0xFFFFFFFFA0000000

