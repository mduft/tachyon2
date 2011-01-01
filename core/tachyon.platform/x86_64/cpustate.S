# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

#include <tachyon.platform/x86/asm/x86_64.h>

.global cpustate_save
.global cpustate_restore

# .----------------------------------------.
# | CPU state save/restore.                |
# '----------------------------------------'

cpustate_save:
    # following stack state is assumed, from uppermost downwards:
    #  -3) (pushed in here) original RFLAGS
    #  -2) (pushed in here) original RBX
    #  -1) (pushed in here) original RAX
    #   0) return address.
    #   1) address of a cpustate_t storage
    #   2) RIP to save
    #   3) RSP to save

    # ATTENTION: this has exact knowledge of the layout of the
    # cpustate_t, so be _VERY_ careful, when adjusting it!

    push %rax                   # need to save the original rax on the stack.
    push %rbx                   # need to save the original rbx on the stack.
    pushf                       # save rflags

    mov 0x20(%rsp), %rax        # get the address of the cpustate_t in rax

    mov 0x28(%rsp), %rbx
    mov %rbx      ,     (%rax)  # store RIP

    mov (%rsp)    , %rbx
    mov %rbx      , 0x08(%rax)  # store RFLAGS

    mov 0x10(%rsp), %rbx
    mov %rbx      , 0x10(%rax)  # store RAX
    mov %rcx      , 0x18(%rax)  # store RCX
    mov %rdx      , 0x20(%rax)  # store RDX

    mov 0x08(%rsp), %rbx
    mov %rbx      , 0x28(%rax)  # store RBX

    mov 0x30(%rsp), %rbx
    mov %rbx      , 0x30(%rax)  # store RSP
    mov %rbp      , 0x38(%rax)  # store RBP
    mov %rsi      , 0x40(%rax)  # store RSI
    mov %rdi      , 0x48(%rax)  # store RDI

    mov %es       , 0x50(%rax)  # store ES
    mov %cs       , 0x52(%rax)  # store CS
    mov %ss       , 0x54(%rax)  # store SS
    mov %ds       , 0x56(%rax)  # store DS
    mov %fs       , 0x58(%rax)  # store FS
    mov %gs       , 0x5a(%rax)  # store GS

    mov %r8       , 0x60(%rax)
    mov %r9       , 0x68(%rax)
    mov %r10      , 0x70(%rax)
    mov %r11      , 0x78(%rax)
    mov %r12      , 0x80(%rax)
    mov %r13      , 0x88(%rax)
    mov %r14      , 0x90(%rax)
    mov %r15      , 0x98(%rax)

    pop %rbx
    pop %rbx
    pop %rbx

    # rax already has the addr of the structure in it!
    ret
