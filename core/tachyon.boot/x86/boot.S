# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

#include <tachyon.platform/x86/asm/x86.h>

.global bootstrap_x86
.global abort

# .------------------------------.
# | DATA Section                 |
# '------------------------------'

.section .data

# the kernel page directory, lowest page table (for the low 1MB identity 
# mapping in kernel space) and the kernel page table, used to map the
# kernel itself from physical 0x100000 to 0xC0000000).

.align 0x1000
# a whopping 64K of initial stack space.
x86_core_stack:
    .space 0x10000, 0x00
    x86_core_stack_top:

# .------------------------------.
# | TEXT Section                 |
# '------------------------------'

SECTION_BOOT_TEXT
.code32

.set ALIGN,         1<<0             # align loaded modules on page boundaries
.set MEMINFO,       1<<1             # provide memory map
.set KLUDGE,        1<<16            # use a.out kludge (allows loading of section headers, etc.)
.set FLAGS,         ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,         0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM,      -(MAGIC + FLAGS) # checksum required

.align 4
x86_mbhdr:
    .long MAGIC
    .long FLAGS
    .long CHECKSUM
    .long x86_mbhdr

bootstrap_x86:
    # setup the stack
    mov   $x86_core_stack_top, %esp

    # adjust address to be physical, stack is in data segment, which is linked to
    # the kernels virtual higher half address.
    subl  $CORE_VMA_X86, %esp

    # setup a 32 bit gdt.
    lgdt  (x86_gdt_ptr)

    ljmp $0x8,$1f
1:

    push %eax
    mov $0x10, %ax              # set propper data segment selectors.
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # setup boot paging to map kernel to higher half

    #mov %cr4, %eax               # read CR4
    #or $CR4_PAE, %eax            # set the PAE bit.
    #mov %eax, %cr4               # and write it back.

    mov $x86_pg_pd, %eax         # load 32 bit PD
    mov %eax, %cr3               # and activate it.
    mov %cr0, %eax               # get current cr0
    or $CR0_PAGING, %eax         # set paging bit
    or $CR0_WRITE_PROTECT, %eax  # enable global write protection
    mov %eax, %cr0               # and set the new value

    mov %cr4, %eax               # again, load CR4
    or $CR4_GLOBAL_PAGES, %eax   # must be done after setting CR0_PAGING!
    mov %eax, %cr4               # and do it.
    pop %eax

    # adjust stack registers to point to the now mapped stack as virtual address
    addl  $CORE_VMA_X86, %esp

    # terminate stack with zero return address, and zero base pointer.
    push $0x0
    push $0x0

    mov   %esp, %ebp

    # push parameters to the entry point (grub parameters)
    push  %eax
    push  %ebx

    # .------------------------------------.
    # | Main kernel entry (C/C++)          |
    # '------------------------------------'

    call __init_bss
    call __kcon_reset
    call __init_cxx

    call x86_idt_init
    lidt (x86_idt_ptr)

    call boot

    call __cleanup_cxx

    push $0x0
    call __cxa_finalize

    # .------------------------------------.
    # | the end... this should prolly never|
    # | happen if the kernel behaves...    |
    # '------------------------------------'

    call abort

.section .data

abort_cpustate:
    # over-reserve, to be bullet-proof on changes.
    .space 0x100

.section .text

abort:
    # prepare cpu state pointer and extended cpu state pointer.
    # the return address on the stack is taken as EIP

    push %eax               # save original EAX
    mov %esp, %eax          # current ESP
    add $0x08, %eax         # minus saved EAX and return address
    push %eax               # original ESP

    mov 0x08(%esp), %eax    # the return address
    push %eax               # original EIP
    mov 0x08(%esp), %eax    # original EAX

    push $abort_cpustate
    call cpustate_save

    pop %ebx
    pop %ebx
    pop %ebx

    push %eax

    movw $0x0c00, __kcon_attr # make the output bright red on the console.

    call __dump_cpustate
    call __dump_stack

    # halt the cpu... the kernel stopped.
    1:
        cli
        hlt
        jmp 1b

    # no need to clean up the stack any further...
