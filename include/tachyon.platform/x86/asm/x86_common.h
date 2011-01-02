/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

/* 
 * .--------------------------------.
 * | Model Specific Registers       |
 * '--------------------------------' 
 */

#define IA32_APIC_BASE  0x0000001B
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
 * | Interrupt related things       |
 * '--------------------------------' 
 */

#define IDTT_SMALL_TSS_AVAIL    (0x1 << 8)
#define IDTT_LDT                (0x2 << 8)
#define IDTT_SMALL_TSS_BUSY     (0x3 << 8)
#define IDTT_SMALL_CALL_GATE    (0x4 << 8)
#define IDTT_TASK_GATE          (0x5 << 8)
#define IDTT_SMALL_INT_GATE     (0x6 << 8)
#define IDTT_SMALL_TRAP_GATE    (0x7 << 8)
#define IDTT_LARGE_TSS_AVAIL    (0x9 << 8)
#define IDTT_LARGE_TSS_BUSY     (0xb << 8)
#define IDTT_LARGE_CALL_GATE    (0xc << 8)
#define IDTT_LARGE_INT_GATE     (0xe << 8)
#define IDTT_LARGE_TRAP_GATE    (0xf << 8)

#define IDTE_PRESENT            (0x1 << 15)

/* 
 * .--------------------------------.
 * | Paging related things          |
 * '--------------------------------' 
 */

#define PAGE_PRESENT            (1 << 0)
#define PAGE_WRITABLE           (1 << 1)
#define PAGE_USER               (1 << 2)
#define PAGE_WRITETHROUGH       (1 << 3)
#define PAGE_NONCACHABLE        (1 << 4)
#define PAGE_ACCESSED           (1 << 5)
#define PAGE_DIRTY              (1 << 6)
#define PAGE_LARGE              (1 << 7)
#define PAGE_GLOBAL             (1 << 8)
#define PAGE_EXECUTE_DISABLE    (1 << 63)

#define PAGE_SIZE_4K            0x001000
#define PAGE_SIZE_2M            0x200000
#define PAGE_SIZE_4M            0x400000

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

/* 
 * .--------------------------------.
 * | Control register possible vals |
 * '--------------------------------' 
 */

#define CR0_PAGING              (1 << 31)
#define CR0_CACHE_DISABLE       (1 << 30)
#define CR0_NOT_WRITE_THROUGH   (1 << 29)
#define CR0_ALIGNMENT_MASK      (1 << 18)
#define CR0_WRITE_PROTECT       (1 << 16)
#define CR0_NUMERIC_ERROR       (1 << 5)
#define CR0_EXTENSION_TYPE      (1 << 4)
#define CR0_TASK_SWITCHED       (1 << 3)
#define CR0_FPU_EMULATION       (1 << 2)
#define CR0_MONITOR_FPU         (1 << 1)
#define CR0_PROTECTION          (1 << 0)

#define CR4_VME                 (1 << 0)
#define CR4_PMODE_VIRTUAL_INT   (1 << 1)
#define CR4_TIMESTAMP_RESTRICT  (1 << 2)
#define CR4_DEBUGGING_EXT       (1 << 3)
#define CR4_PSE                 (1 << 4)
#define CR4_PAE                 (1 << 5)
#define CR4_MCE                 (1 << 6)
#define CR4_GLOBAL_PAGES        (1 << 7)
#define CR4_PERF_COUNTER        (1 << 8)
#define CR4_OS_FXSR             (1 << 9)
#define CR4_OS_XMMEXCEPT        (1 << 10)
#define CR4_VMX_ENABLE          (1 << 13)
#define CR4_SMX_ENABLE          (1 << 14)
#define CR4_PCID_ENABLE         (1 << 17)
#define CR4_OS_XSAVE            (1 << 18)

