# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

#include <tachyon.platform/x86/asm/x86_64.h>

SECTION_BOOT_DATA

.global x86_64_idt_ptr
.global x86_64_idt_init
.global x86_64_idt_set
.global x86_64_idt_stack_top

# .----------------------------------------.
# | 64 bit IDT                             |
# '----------------------------------------'

.align 0x10
x86_64_idt:
    .fill 0xFF, 0x8, 0x0
    .fill 0xFF, 0x8, 0x0

x86_64_idt_ptr:
    .word . - x86_64_idt - 1
    .quad x86_64_idt

x86_64_idt_stack:
    .space 0x2000
    x86_64_idt_stack_top:

# .----------------------------------------.
# | 64 bit ISRs that delegate              |
# '----------------------------------------'

.section .text

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

x86_64_isr     X86_DIV_ERR,     0       #DE     0x00
x86_64_isr     X86_DEBUG_EX,    0       #DB     0x01
x86_64_isr     X86_NMI,         0       #NMI    0x02
x86_64_isr     X86_BREAKPOINT,  0       #BP     0x03
x86_64_isr     X86_OVERFLOW,    0       #OF     0x04
x86_64_isr     X86_BOUND_RANGE, 0       #BR     0x05
x86_64_isr     X86_INVALID_OP,  0       #UD     0x06
x86_64_isr     X86_DEVICE_NA,   0       #NM     0x07
x86_64_isr     X86_DFAULT,      1       #DF     0x08
x86_64_isr     X86_CO_SEG_OF,   0,  1   #RESERVED - only in old CPUs
x86_64_isr     X86_INVALID_TSS, 1       #TS     0x0a
x86_64_isr     X86_SEG_NP,      1       #NP     0x0b
x86_64_isr     X86_STACK_FAULT, 1       #SS     0x0c
x86_64_isr     X86_GPF,         1       #GP     0x0d
x86_64_isr     X86_PAGE_FAULT,  1       #PF     0x0e
x86_64_isr     15,              0,  1   #RESERVED
x86_64_isr     X86_FPE,         0       #MF     0x10
x86_64_isr     X86_ALIGN_CHECK, 1       #AC     0x11
x86_64_isr     X86_MCE,         0       #MC     0x12
x86_64_isr     X86_SIMD_FPE,    0       #XM     0x13

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

# .----------------------------------------.
# | Functions to init and fill the idt     |
# '----------------------------------------'

.macro set_int_gate num
    mov $_x86_64_isr\num, %rsi
    mov $\num, %rdi
    call x86_64_idt_set
.endm

x86_64_idt_init:
    push %rbp
    mov %rsp, %rbp
    push %rcx
    push %rsi
    push %rdi

    set_int_gate 0
    set_int_gate 1
    set_int_gate 2
    set_int_gate 3
    set_int_gate 4
    set_int_gate 5
    set_int_gate 6
    set_int_gate 7
    set_int_gate 8
    set_int_gate 9
    set_int_gate 10
    set_int_gate 11
    set_int_gate 12
    set_int_gate 13
    set_int_gate 14
    set_int_gate 15
    set_int_gate 16
    set_int_gate 17
    set_int_gate 18
    set_int_gate 19
    set_int_gate 20
    set_int_gate 21
    set_int_gate 22
    set_int_gate 23
    set_int_gate 24
    set_int_gate 25
    set_int_gate 26
    set_int_gate 27
    set_int_gate 28
    set_int_gate 29
    set_int_gate 30
    set_int_gate 31

    pop %rdi
    pop %rsi
    pop %rcx
    pop %rbp
    ret

x86_64_idt_set:
    push %rbp
    mov %rsp, %rbp
    push %rax
    push %rbx

    # RSI: the target RIP
    # RDI: the gate number

    push %rdx
    mov %rdi, %rax
    mov $0x10, %rdx
    mul %rdx
    pop %rdx

    add $x86_64_idt, %rax

    xor %ebx, %ebx

    mov %rsi, %rbx
    and $0xFFFF, %ebx
    or $(0x18 << 0x10), %ebx

    mov %ebx, (%rax)
    mov %rsi, %rbx
    and $~0xFFFF, %ebx
    or $IDTT_LARGE_INT_GATE, %ebx
    mov %ebx, 4(%rax)

    shr $0x20, %rsi
    mov %rsi, 8(%rax)

#    .word _x86_64_isr##x & 0xFFFF                         # lowest bits of RIP    \
#    .word 0x18                                              # segment selector      \
#    .long _x86_64_isr##x &~0xFFFF | IDTT_LARGE_INT_GATE   # 16:31 of RIP + type   \
#    .long _x86_64_isr##x & 0xFFFFFFFF                     # 32:63 of RIP          \
#    .long 0x0                                               # reserved (zero)

    pop %rbx
    pop %rax
    pop %rbp
    ret
