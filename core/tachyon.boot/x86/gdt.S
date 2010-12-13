# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

#include <tachyon.platform/x86/asm/x86.h>

SECTION_BOOT_DATA

.global ia32_gdt_code
.global ia32_gdt_data
.global ia32_gdt_ptr

# .----------------------------------------.
# | 32 kernel gdt code and data segments   |
# '----------------------------------------'

ia32_gdt:
    .quad 0x0

    ia32_gdt_code:
        .long 0x0000FFFF
        .long 0x000F0000 | (GDT_PRESENT | GDT_TYPE_CODE_OR_DATA | GDT_TYPE_CODE | GDT_CODE_READ_ENABLE | GDT_DEF_OPSIZE_32BIT | GDT_GRANULARITY)
    ia32_gdt_data:
        .long 0x0000FFFF
        .long 0x000F0000 | (GDT_PRESENT | GDT_TYPE_CODE_OR_DATA | GDT_DATA_WRITE_ENABLE | GDT_DEF_OPSIZE_32BIT | GDT_GRANULARITY)

# .----------------------------------------.
# | 32 kernel gdt pointer (limit + ptr)    |
# '----------------------------------------'

ia32_gdt_ptr:
    .word . - ia32_gdt - 1
    .long ia32_gdt

