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
_kernel_pd:
    .space 0x1000, 0x00
_kernel_pt:
    .space 0x1000, 0x00
_kernel_low_pt:
    .space 0x1000, 0x00

# a whopping 64K of initial stack space.
initstack:
    .space 0x10000, 0x00
initstack_top:

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

.set VIDEO_RAM,     0xB8000          # Video Memory, used to print to the screen.
.set VIDEO_DWORDS,  0x3E8            # The count of DWORDs (!) the screen buffer is large.

.align 4

multiboot_header:
    .long MAGIC
    .long FLAGS
    .long CHECKSUM
    .long multiboot_header
    .long CORE_LMA_START
    .long 0
    .long 0
    .long bootstrap_x86

bootstrap_x86:
    # setup the stack
    mov   $initstack_top, %esp

    # adjust address to be physical, stack is in data segment, which is linked to
    # the kernels virtual higher half address.
    subl  $CORE_VMA_X86, %esp

    # setup a 32 bit gdt.
    lgdt  (ia32_gdt_ptr)

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

    mov $ia32_pg_pd, %eax       # load 32 bit PD
    mov %eax, %cr3              # and activate it.
    mov %cr0, %eax              # get current cr0
    bts $31, %eax               # set paging bit
    mov %eax, %cr0              # and set the new value
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

    # TODO: assure that the multiboot information is mapped!

    # .------------------------------------.
    # | Main kernel entry (C/C++)          |
    # '------------------------------------'

    call __init_bss
    call __kcon_reset
    call __init_cxx
    call boot

    call __cleanup_cxx

    push $0x0
    call __cxa_finalize

    # .------------------------------------.
    # | the end... this should prolly never|
    # | happen if the kernel behaves...    |
    # '------------------------------------'

    call abort

temp_store:
    .long 0

abort:
    # prepare cpu state pointer and extended cpu state pointer.
    # the return address on the stack is taken as RIP

    mov %eax, temp_store
    pop %eax

    push %eax   # for frame return
    push %ebp   # frame base pointer
    push %eax   # first field of cpustate_t (rip)

    mov temp_store, %eax
    mov %ebp, temp_store
    mov %esp, %ebp
    add $0x4, %ebp

    pushf
    push %eax
    push %ebx

    mov %esp, %eax
    add $(4 * 0x4), %eax   # contains original stack pointer, before the cpu state

    push %ecx
    push %edx
    push %esi
    push %edi
    push %eax   # the actual original stack pointer
    mov temp_store, %eax
    push %eax

    mov %cr0, %eax
    push %eax

    mov %cr2, %eax
    push %eax

    mov %cr3, %eax
    push %eax

    mov %cr4, %eax
    push %eax

    mov %esp, %eax

    push $0x0
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
