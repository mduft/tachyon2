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

.set ALIGN,         1<<0             # align loaded modules on page boundaries
.set MEMINFO,       1<<1             # provide memory map
.set KLUDGE,        1<<16            # use a.out kludge (allows loading of section headers, etc.)
.set FLAGS,         MEMINFO | KLUDGE | ALIGN
.set MAGIC,         0x1BADB002
.set CHECKSUM,      -(MAGIC + FLAGS)

# multiboot header goes here, not into data section,
# since the header needs to be somewhere in the front,
# and the code too.

.align 4
bootstrap_x86_64_mbhdr:
    .long MAGIC                     # the magic number
    .long FLAGS                     # the loader flags
    .long CHECKSUM                  # checksum
    .long bootstrap_x86_64_mbhdr    # address of this header
    .long CORE_LMA_START            # start of the kernel
    .long _core_lma_edata           # end of data. kernel (file!) must be at least this big.
    .long _core_lma_ebss            # end of bss, which might not physically exist in file.
    .long bootstrap_x86_64          # entry point address

bootstrap_x86_64:
    # first, let's start witch setting up a stack in lower half.
    mov $bootstrap_x86_64_stack_top, %esp

    # save the boot-loader-set eax & ebx
    mov %eax, (bootstrap_x86_64_boot_eax)
    mov %ebx, (bootstrap_x86_64_boot_ebx)

    lgdt (x86_64_gdt_ptr)        # load the new 64-bit gdt.

    ljmp $0x8,$1f
1:

    mov $0x10, %ax              # set propper data segment selectors.
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # disable the good ole 8259A PIC, we'll use the APIC instead
    mov $0xFF, %al
    out %al, $0xA1
    out %al, $0x21

    # Assume, that paging structures are correct here, we
    # only need to switch to long mode, and enable paging

    mov %cr4, %eax              # read CR4
    or $CR4_PAE, %eax           # set the PAE bit.
    mov %eax, %cr4              # and write it back.

    mov $IA32_EFER, %ecx        # read extended feature enabled register
    rdmsr                       # do it.
    bts  $8, %eax               # set the LME bit.
    wrmsr                       # and write it back.

    # We're in x86_64 compatability mode now. We now have to
    # enable paging, and activate a 64 bit gdt.

    mov $x86_64_pg_pml4, %eax   # load the boot paging PML4
    mov %eax, %cr3              # set the new PML4
    mov %cr0, %eax              # read CR0
    or $CR0_PAGING, %eax        # set the paging enabled bit.
    or $CR0_WRITE_PROTECT, %eax # enable write protection system wide for W/P pages.
    mov %eax, %cr0              # and write it back

    mov %cr4, %eax              # again, load CR4
    bts $CR4_GLOBAL_PAGES, %eax # must be done after setting CR0_PAGING!
    mov %eax, %cr4              # and do it.

    ljmp $0x18,$bootstrap_x86_64_x64

# .----------------------------------------.
# | 64 bit init code. enables paging.      |
# '----------------------------------------'

.code64
bootstrap_x86_64_x64:
    movq $bootstrap_x86_64_stack_top, %rsp
    movq $CORE_VMA_X86_64, %rax
    addq %rax, %rsp

    pushq $0x0
    pushq $0x0
    movq %rsp, %rbp

    call __init_bss
    call __kcon_reset

    call x86_64_idt_init
    lidt (x86_64_idt_ptr)

    call __init_cxx

    # restore the multiboot-loader information to the correct register
    mov (bootstrap_x86_64_boot_ebx), %rdi
    mov (bootstrap_x86_64_boot_eax), %rsi

    # TODO: assure that the multiboot information is mapped!

    call boot

    call __cleanup_cxx

    xorq %rdi, %rdi
    call __cxa_finalize
    call abort

.section .data

abort_cpustate:
    # over-reserve, to be future bullet-proof...
    .space 0x100

.section .text

abort:
    # prepare cpu state pointer and extended cpu state pointer.
    # the return address on the stack is taken as RIP

    push %rax               # save original RAX
    mov %rsp, %rax          # current RSP
    add $0x10, %rax         # minus saved RAX and return address
    push %rax               # original RSP

    mov 0x10(%rsp), %rax    # the return address
    push %rax               # original RIP
    mov 0x10(%rsp), %rax    # original RAX

    push $abort_cpustate
    call cpustate_save
    mov %rax, %rdi

    pop %rax
    pop %rax
    pop %rax

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

bootstrap_x86_64_stack:
    .space 0x10000
    bootstrap_x86_64_stack_top:

bootstrap_x86_64_boot_eax:
    .long 0x0

bootstrap_x86_64_boot_ebx:
    .long 0x0

# .----------------------------------------.
# | Data required in higher half.          |
# '----------------------------------------'

.section .data

_stop_str:
    .asciz "\nstopped!"
