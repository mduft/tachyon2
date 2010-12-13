/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include "types.h"

/* helper macros to encode/decode the info (contains symbol index and type) */
#if ELF_BITS == 64
# define RI_DECODE(i, s, t)  s = ((i)>>32); t = ((i)&0xFFFFFFFF);
# define RI_ENCODE(i, s, t)  i = (((s)<<32) + ((t)&0xFFFFFFFF));
#elif ELF_BITS == 32
# define RI_DECODE(i, s, t)  s = ((i)>>8); t = ((i)&0xFF);
# define RI_ENCODE(i, s, t)  i = (((s)<<8) + ((t)&0xFF));
#endif

#define R_386_NONE      0
#define R_386_32        1
#define R_386_PC32      2
#define R_386_GOT32     3
#define R_386_PLT32     4
#define R_386_COPY      5
#define R_386_GLOB_DAT  6
#define R_386_JMP_SLOT  7
#define R_386_RELATIVE  8
#define R_386_GOTOFF    9
#define R_386_GOTPC     10

typedef struct
{
    Elf_Addr      r_offset;
    Elf_Xword     r_info;
} PACKED Elf_Rel;

typedef struct
{
    Elf_Addr      r_offset;
    Elf_Xword     r_info;
    Elf_Sxword    r_addend;
} PACKED Elf_Rela;

