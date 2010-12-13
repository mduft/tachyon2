# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

#include <tachyon.platform/x86/asm/x86_64.h>

.global bootstrap_x86_64
.global abort

# .----------------------------------------.
# | 32 bit init code. switches to 64-bit.  |
# '----------------------------------------'

SECTION_BOOT_TEXT
.code32

.set MEMINFO,       1<<1
.set KLUDGE,        1<<16
.set FLAGS,         MEMINFO | KLUDGE
.set MAGIC,         0x1BADB002
.set CHECKSUM,      -(MAGIC + FLAGS)

# multiboot header goes here, not into data section,
# since the header needs to be somewhere in the front,
# and the code too.

.align 4
bootstrap_ia32e_mbhdr:
    .long MAGIC                     # the magic number
    .long FLAGS                     # the loader flags
    .long CHECKSUM                  # checksum
    .long bootstrap_ia32e_mbhdr     # address of this header
    .long CORE_LMA_START            # start of the kernel
    .long 0x0                       # load all of the image (no end of data section)
    .long 0x0                       # load all image, and clear bss ourselves
    .long bootstrap_x86_64          # entry point address

bootstrap_x86_64:
    # first, let's start witch setting up a stack in lower half.
    mov $bootstrap_ia32e_stack_top, %esp

    # save the boot-loader-set eax & ebx
    mov %eax, (bootstrap_ia32e_boot_eax)
    mov %ebx, (bootstrap_ia32e_boot_ebx)

    lgdt (ia32e_gdt_ptr)        # load the new 64-bit gdt.

    ljmp $0x8,$1f
1:

    mov $0x10, %ax              # set propper data segment selectors.
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # Assume, that paging structures are correct here, we
    # only need to switch to long mode, and enable paging

    mov %cr4, %eax              # read CR4
    bts $5, %eax                # set the PAE bit.
    mov %eax, %cr4              # and write it back.

    mov $IA32_EFER, %ecx        # read extended feature enabled register
    rdmsr                       # do it.
    bts  $8, %eax               # set the LME bit.
    wrmsr                       # and write it back.

    # We're in ia32e compatability mode now. We now have to
    # enable paging, and activate a 64 bit gdt.

    mov $ia32e_pg_pml4, %eax    # load the boot paging PML4
    mov %eax, %cr3              # set the new PML4
    mov %cr0, %eax              # read CR0
    bts $31, %eax               # set the paging enabled bit.
    mov %eax, %cr0              # and write it back

    ljmp $0x18,$bootstrap_ia32e_x64

# .----------------------------------------.
# | 64 bit init code. enables paging.      |
# '----------------------------------------'

.code64
bootstrap_ia32e_x64:
    movq $bootstrap_ia32e_stack_top, %rsp
    movq $CORE_VMA_X86_64, %rax
    addq %rax, %rsp

    pushq $0x0
    pushq $0x0
    movq %rsp, %rbp

    call x86_64_idt_init
    # TODO: 64 bit TSS, 64bit ISTs and then activate this.
    #lidt (x86_64_idt_ptr)

    # TESTTEST
    #mov 0x0, %rax

    call __init_bss
    call __kcon_reset
    call __init_cxx

    # restore the multiboot-loader information to the correct register
    mov (bootstrap_ia32e_boot_ebx), %rdi
    mov (bootstrap_ia32e_boot_eax), %rsi

    # TODO: assure that the multiboot information is mapped!

    call boot

    call __cleanup_cxx

    xorq %rdi, %rdi
    call __cxa_finalize
    call abort

temp_store:
    .quad 0

abort:
    # prepare cpu state pointer and extended cpu state pointer.
    # the return address on the stack is taken as RIP

    mov %rax, temp_store
    pop %rax

    push %rax   # for frame return
    push %rbp   # frame base pointer
    push %rax   # first field of cpustate_t (rip)

    mov temp_store, %rax
    mov %rbp, temp_store
    mov %rsp, %rbp
    add $0x8, %rbp

    pushf
    push %rax
    push %rbx

    mov %rsp, %rax
    addq $(4 * 0x8), %rax   # contains original stack pointer, before the cpu state

    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rax   # the actual original stack pointer
    mov temp_store, %rax
    push %rax

    mov %cr0, %rax
    push %rax

    mov %cr2, %rax
    push %rax

    mov %cr3, %rax
    push %rax

    mov %cr4, %rax
    push %rax

    # extended state

    mov %rsp, %rdi  # argument one: the pointer to cpu state

    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    mov %rsp, %rsi  # argument two: the pointer to extended cpu state
    movw $0x0c00, __kcon_attr # make the output bright red on the console.

    call __dump_cpustate
    call __dump_stack

    1:
        cli
        hlt
        jmp 1b

# .----------------------------------------.
# | Data required in lower half.           |
# '----------------------------------------'

SECTION_BOOT_DATA

bootstrap_ia32e_stack:
    .space 0x10000
    bootstrap_ia32e_stack_top:

bootstrap_ia32e_boot_eax:
    .long 0x0

bootstrap_ia32e_boot_ebx:
    .long 0x0

# .----------------------------------------.
# | Data required in higher half.          |
# '----------------------------------------'

.section .data

_stop_str:
    .asciz "\nstopped!"
