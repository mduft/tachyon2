# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

#include <tachyon.platform/x86/asm/x86_64.h>

SECTION_BOOT_DATA

.global x86_64_idt_ptr

# .----------------------------------------.
# | 64 bit IDT                             |
# '----------------------------------------'

# 00-19 = CPU exceptions
# 20-31 = Reserved
# 32-255 = Available

# TODO: real IDT entry setting.
.macro x86_64_idtes from, to
    .if \to-\from
        x86_64_idtes "(\from + 1)", \to
    .endif
.endm

x86_64_idt:
    x86_64_idtes 0, 31

x86_64_idt_ptr:
    .word . - x86_64_idt - 1
    .quad x86_64_idt

# .----------------------------------------.
# | 64 bit ISRs that delegate              |
# '----------------------------------------'

x86_64_isr_trampoline:
    # all ISRs land here, with a possible dummy error code.
    iret

.macro x86_64_isr num, has_err, res=0
_x86_64_isr\num:
    .if !\has_err
    push $0x0
    .endif
    .if \res
    call abort
    .else
    jmp x86_64_isr_trampoline
    .endif
.endm

x86_64_isr     X86_DIV_ERR,     0       #DE
x86_64_isr     X86_DEBUG_EX,    0       #DB
x86_64_isr     X86_NMI,         0       #NMI
x86_64_isr     X86_BREAKPOINT,  0       #BP
x86_64_isr     X86_OVERFLOW,    0       #OF
x86_64_isr     X86_BOUND_RANGE, 0       #BR
x86_64_isr     X86_INVALID_OP,  0       #UD
x86_64_isr     X86_DEVICE_NA,   0       #NM
x86_64_isr     X86_DFAULT,      1       #DF
x86_64_isr     X86_CO_SEG_OF,   0,  1   #RESERVED - only in old CPUs
x86_64_isr     X86_INVALID_TSS, 1       #TS
x86_64_isr     X86_SEG_NP,      1       #NP
x86_64_isr     X86_STACK_FAULT, 1       #SS
x86_64_isr     X86_GPF,         1       #GP
x86_64_isr     X86_PAGE_FAULT,  1       #PF
x86_64_isr     15,              0,  1   #RESERVED
x86_64_isr     X86_FPE,         0       #MF
x86_64_isr     X86_ALIGN_CHECK, 1       #AC
x86_64_isr     X86_MCE,         0       #MC
x86_64_isr     X86_SIMD_FPE,    0       #XM

x86_64_isr     20,              0,  1   #RESERVED
x86_64_isr     21,              0,  1   #RESERVED
x86_64_isr     22,              0,  1   #RESERVED
x86_64_isr     23,              0,  1   #RESERVED
x86_64_isr     24,              0,  1   #RESERVED
x86_64_isr     25,              0,  1   #RESERVED
x86_64_isr     26,              0,  1   #RESERVED
x86_64_isr     27,              0,  1   #RESERVED
x86_64_isr     28,              0,  1   #RESERVED
x86_64_isr     29,              0,  1   #RESERVED
x86_64_isr     30,              0,  1   #RESERVED
x86_64_isr     31,              0,  1   #RESERVED
