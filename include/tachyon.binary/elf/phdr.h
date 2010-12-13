/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include "types.h"

#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define PT_NOTE     4
#define PT_SHLIB    5
#define PT_PHDR     6

#define PF_X        0x1
#define PF_W        0x2
#define PF_R        0x4

#if ELF_BITS == 32
typedef struct {
    Elf_Word    p_type;
    Elf_Off     p_offset;
    Elf_Addr    p_vaddr;
    Elf_Addr    p_paddr;
    Elf_Word    p_filesz;
    Elf_Word    p_memsz;
    Elf_Word    p_flags;
    Elf_Word    p_align;
} PACKED Elf_Phdr;
#elif ELF_BITS == 64
typedef struct
{
    Elf_Word    p_type;
    Elf_Word    p_flags;
    Elf_Off     p_offset;
    Elf_Addr    p_vaddr;
    Elf_Addr    p_paddr;
    Elf_Xword   p_filesz;
    Elf_Xword   p_memsz;
    Elf_Xword   p_align;
} PACKED Elf_Phdr;
#endif
