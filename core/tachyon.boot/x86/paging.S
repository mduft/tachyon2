# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

#include <tachyon.platform/x86/asm/x86.h>

SECTION_BOOT_DATA

.global x86_pg_pdpt
.global x86_temp_mapspace

#define PG_BASE_KFLAGS  ( PAGE_PRESENT | PAGE_WRITABLE | PAGE_GLOBAL )

# .----------------------------------------.
# | PT's, currently only for low memory,   |
# | taking care of not mapping NULL, and   |
# | identity mapping the rest.             |
# '----------------------------------------'

.macro x86_pte cnt addr
.if \cnt
    .quad \addr + ( PG_BASE_KFLAGS )
    x86_pte "(\cnt - 1)", "(\addr + 0x1000)"
.endif
.endm

# 4KB - 4MB
.align 0x1000
x86_pg_pt:
    .quad 0x0
    x86_pte 0x64, 0x00001000
    x86_pte 0x64, 0x00065000
    x86_pte 0x64, 0x000C9000
    x86_pte 0x64, 0x0012D000
    x86_pte 0x64, 0x00191000
    x86_pte 0x0B, 0x001F5000

# .----------------------------------------.
# | PD's, mapping 4MB pages, except low1,  |
# | which needs to make sure that the low  |
# | 4KB (NULL!) are unmapped!              |
# '----------------------------------------'

.align 0x1000
x86_pg_pd_low:
    .long 0x0
    .long x86_pg_pt + ( PG_BASE_KFLAGS )                # 0 - 2MB
    .quad 0x200000  + ( PG_BASE_KFLAGS | PAGE_LARGE )   # 2 - 4MB
    .fill 0x1FE, 0x8, 0x0                               # not present

.align 0x1000
x86_pg_pd_high:
    .quad 0x000000  + ( PG_BASE_KFLAGS | PAGE_LARGE )   # 0 - 2MB -> HIGH
    .quad 0x200000  + ( PG_BASE_KFLAGS | PAGE_LARGE )   # 2 - 4MB -> HIGH
    .fill 0x1F4, 0x8, 0x0                               # not present
    x86_temp_mapspace:
    .fill 0xa, 0x8, 0x0                                 # -20MB - -0MB reserved for temp map.

# .----------------------------------------.
# | PDPT, 4 entries large for PAE paging.  |
# '----------------------------------------'

.align 0x1000
x86_pg_pdpt:
    .long 0x0
    .long x86_pg_pd_low + ( PG_BASE_KFLAGS )            # 0 - 1GB
    .quad 0x0                                           # 1 - 2GB
    .quad 0x0                                           # 2 - 3GB
    .long 0x0
    .long x86_pg_pd_high + ( PG_BASE_KFLAGS )           # 3 - 4GB

