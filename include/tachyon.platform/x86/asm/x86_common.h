/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

/* 
 * .--------------------------------.
 * | Model Specific Registers       |
 * '--------------------------------' 
 */

#define IA32_EFER       0xC0000080

/* 
 * .--------------------------------.
 * | Segmentation related things    |
 * '--------------------------------' 
 */

#define GDT_TYPE_CODE_OR_DATA   (1 << 12)
#define GDT_DPL_0               (0)
#define GDT_DPL_1               (1 << 13)
#define GDT_DPL_2               (2 << 13)
#define GDT_DPL_3               (3 << 13)
#define GDT_PRESENT             (1 << 15)
#define GDT_64BIT               (1 << 21)
#define GDT_DEF_OPSIZE_32BIT    (1 << 22)
#define GDT_GRANULARITY         (1 << 23)

#define GDT_TYPE_CODE           (1 << 11)
#define GDT_DATA_EXPAND_DOWN    (1 << 10)
#define GDT_DATA_WRITE_ENABLE   (1 << 9)

#define GDT_ACCESSED            (1 << 8)

#define GDT_CODE_CONFORMING     (1 << 10)
#define GDT_CODE_READ_ENABLE    (1 << 9)

/* 
 * .--------------------------------.
 * | Paging related things          |
 * '--------------------------------' 
 */

#define PAGE_PRESENT            (1 << 0)
#define PAGE_WRITABLE           (1 << 1)
#define PAGE_LARGE              (1 << 7)

/* 
 * .--------------------------------.
 * | Assembler only, section attrs  |
 * '--------------------------------' 
 */

#define SECTION_BOOT_DATA .section .boot_data, "aw", @progbits
#define SECTION_BOOT_TEXT .section .boot_text, "ax"

