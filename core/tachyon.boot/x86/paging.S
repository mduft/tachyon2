# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

#include <tachyon.platform/x86/asm/x86.h>

SECTION_BOOT_DATA

.global ia32_pg_pd

# .----------------------------------------.
# | PT's, currently only for low memory,   |
# | taking care of not mapping NULL, and   |
# | identity mapping the rest.             |
# '----------------------------------------'

.macro ia32_pte cnt addr
.if \cnt
    .long \addr + (PAGE_PRESENT | PAGE_WRITABLE)
    ia32_pte "(\cnt - 1)", "(\addr + 0x1000)"
.endif
.endm

# 4KB - 4MB
.align 0x1000
ia32_pg_pt:
    .long 0x0
    ia32_pte 0x64, 0x00001000
    ia32_pte 0x64, 0x00065000
    ia32_pte 0x64, 0x000C9000
    ia32_pte 0x64, 0x0012D000
    ia32_pte 0x64, 0x00191000
    ia32_pte 0x64, 0x001F5000
    ia32_pte 0x64, 0x00259000
    ia32_pte 0x64, 0x002BD000
    ia32_pte 0x64, 0x00321000
    ia32_pte 0x64, 0x00385000
    ia32_pte 0x17, 0x003E9000

# .----------------------------------------.
# | PD's, mapping 4MB pages, except low1,  |
# | which needs to make sure that the low  |
# | 4KB (NULL!) are unmapped!              |
# '----------------------------------------'

# 0MB - 4MB, rest (until 4GB) unmapped
.align 0x1000
ia32_pg_pd:
    .long ia32_pg_pt + ( PAGE_PRESENT | PAGE_WRITABLE ) # 0 - 4MB
    .fill 0x2FF, 0x4, 0x0 # 767 not present entries.
    .long ia32_pg_pt + ( PAGE_PRESENT | PAGE_WRITABLE )  # higher half map
    .fill 0x0FF, 0x4, 0x0 # 255 not present entries.

