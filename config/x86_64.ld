/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

ENTRY(bootstrap_x86_64)

#include <tachyon.platform/x86/asm/x86_64.h>

CORE_LMA_START = CORE_LMA_X86_64;
CORE_VMA_OFF   = CORE_VMA_X86_64;

#include "common-layout.ld"
