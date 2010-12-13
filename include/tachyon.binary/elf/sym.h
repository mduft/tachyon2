/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include "types.h"

#define STN_UNDEF       0   // index zero is a undefined symbol

#define STB_LOCAL       0   // local to object file (static)
#define STB_GLOBAL      1   // global symbol
#define STB_WEAK        2   // global but lower precedence than globals.

#define STT_NOTYPE      0   // no symbol type
#define STT_OBJECT      1   // data object
#define STT_FUNC        2   // function entry point
#define STT_SECTION     3   // section
#define STT_FILE        4   // file spec.

/* helper macros to decode/encode the st_info field 
 * (contains both binding and type). */
#define STI_DECODE(i, b, t) b = ((i)>>4); t = ((i)&0xF);
#define STI_ENCODE(i, b, t) i = (((b)<<4)+((t)&0xF));

#if ELF_BITS == 32
typedef struct {
    Elf_Word        st_name;
    Elf_Addr        st_value;
    Elf_Word        st_size;
    unsigned char   st_info;
    unsigned char   st_other;
    Elf_Half        str_shndx;
} PACKED Elf_Sym;
#elif ELF_BITS == 64
typedef struct
{
    Elf_Word        st_name;
    unsigned char   st_info;
    unsigned char   st_other;
    Elf_Half        st_shndx;
    Elf_Addr        st_value;
    Elf_Xword       st_size;
} PACKED Elf_Sym;
#endif

