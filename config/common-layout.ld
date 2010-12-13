/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

SECTIONS {
    . = CORE_LMA_START + SIZEOF_HEADERS;

    .bootstrap : {
        *(.boot_text)
        *(.boot_data)
    }

    . += CORE_VMA_OFF;

    .text ALIGN(0x1000) : AT(ADDR(.text) - CORE_VMA_OFF) {
        *(.text*)
        *(.gnu.linkonce.t*)
        . = ALIGN(0x8);
        *(.rodata*)
        *(.gnu.linkonce.r*)

        . = ALIGN(0x8);
        _core_vma_ctors = .;
        *(.ctor*)
        _core_vma_ectors = .;

        . = ALIGN(0x8);
        _core_vma_dtors = .;
        *(.dtor*)
        _core_vma_edtors = .;
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - CORE_VMA_OFF) {
        *(.data)
        *(.gnu.linkonce.d*)

        _core_vma_edata = .;
        _core_lma_edata = . - CORE_VMA_OFF;
    }

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - CORE_VMA_OFF) {
        *(COMMON)
        *(.bss)
        . = ALIGN(0x1000);
        _core_vma_ebss = .;
        _core_lma_ebss = . - CORE_VMA_OFF;
    }

    _core_vma_end = .;
    _core_lma_end = . - CORE_VMA_OFF;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
