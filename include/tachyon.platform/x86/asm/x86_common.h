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
 * | Assembler section attributes   |
 * '--------------------------------' 
 */

#define SECTION_BOOT_DATA .section .boot_data, "aw", @progbits
#define SECTION_BOOT_TEXT .section .boot_text, "ax"

/* 
 * .--------------------------------.
 * | CPU Exceptions                 |
 * '--------------------------------' 
 */

#define X86_DIV_ERR     0
#define X86_DEBUG_EX    1
#define X86_NMI         2
#define X86_BREAKPOINT  3
#define X86_OVERFLOW    4
#define X86_BOUND_RANGE 5
#define X86_INVALID_OP  6
#define X86_DEVICE_NA   7
#define X86_DFAULT      8
#define X86_CO_SEG_OF   9
#define X86_INVALID_TSS 10
#define X86_SEG_NP      11
#define X86_STACK_FAULT 12
#define X86_GPF         13
#define X86_PAGE_FAULT  14
#define X86_FPE         16
#define X86_ALIGN_CHECK 17
#define X86_MCE         18
#define X86_SIMD_FPE    19