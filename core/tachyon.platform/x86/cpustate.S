# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

#include <tachyon.platform/x86/asm/x86.h>

.global cpustate_save
.global cpustate_restore

# .----------------------------------------.
# | CPU state save/restore.                |
# '----------------------------------------'

cpustate_save:
    # following stack state is assumed, from uppermost downwards:
    #  -3) (pushed in here) original EFLAGS
    #  -2) (pushed in here) original EBX
    #  -1) (pushed in here) original EAX
    #   0) return address.
    #   1) address of a cpustate_t storage
    #   2) EIP to save
    #   3) ESP to save

    # ATTENTION: this has exact knowledge of the layout of the
    # cpustate_t, so be _VERY_ careful, when adjusting it!

    push %eax                   # need to save the original rax on the stack.
    push %ebx                   # need to save the original rbx on the stack.
    pushf                       # save rflags

    mov 0x10(%esp), %eax        # get the address of the cpustate_t in rax

    mov 0x14(%esp), %ebx
    mov %ebx      ,     (%eax)  # store RIP

    mov (%esp)    , %ebx
    mov %ebx      , 0x04(%eax)  # store RFLAGS

    mov 0x08(%esp), %ebx
    mov %ebx      , 0x08(%eax)  # store RAX
    mov %ecx      , 0x0c(%eax)  # store RCX
    mov %edx      , 0x10(%eax)  # store RDX

    mov 0x04(%esp), %ebx
    mov %ebx      , 0x14(%eax)  # store RBX

    mov 0x18(%esp), %ebx
    mov %ebx      , 0x18(%eax)  # store RSP
    mov %ebp      , 0x1c(%eax)  # store RBP
    mov %esi      , 0x20(%eax)  # store RSI
    mov %edi      , 0x24(%eax)  # store RDI

    mov %es       , 0x28(%eax)  # store ES
    mov %cs       , 0x2a(%eax)  # store CS
    mov %ss       , 0x2c(%eax)  # store SS
    mov %ds       , 0x2e(%eax)  # store DS
    mov %fs       , 0x30(%eax)  # store FS
    mov %gs       , 0x32(%eax)  # store GS

    pop %ebx
    pop %ebx
    pop %ebx

    # eax already has the addr of the structure in it!
    ret
