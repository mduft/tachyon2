/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include "types.h"

#define SHN_UNDEF   0       // undefined section
#define SHN_ABS     0xFFF1  // absolute symbols
#define SHN_COMMON  0xFFF2  // COMMON symbol reference.

#define SHT_NULL        0   // inacitve header
#define SHT_PROGBITS    1   // program specific stuff
#define SHT_SYMTAB      2   // symbol table
#define SHT_STRTAB      3   // string table
#define SHT_RELA        4   // relocation information with addends
#define SHT_HASH        5   // symbol hash table
#define SHT_DYNAMIC     6   // dynamic linking information
#define SHT_NOTE        7   // notes.
#define SHT_NOBITS      8   // zero sized in file
#define SHT_REL         9   // relocation information without addends
#define SHT_SHLIB       10  // reserved/unspecified
#define SHT_DYNSYM      11  // dynamic symbol table

#define SHF_WRITE       0x1 // writable data
#define SHF_ALLOC       0x2 // should be loaded for execution
#define SHF_EXECINSTR   0x4 // executable instructions

typedef struct
{
    Elf_Word    sh_name;
    Elf_Word    sh_type;
    Elf_Xword   sh_flags;
    Elf_Addr    sh_addr;
    Elf_Off     sh_offset;
    Elf_Xword   sh_size;
    Elf_Word    sh_link;
    Elf_Word    sh_info;
    Elf_Xword   sh_addralign;
    Elf_Xword   sh_entsize;
} PACKED Elf_Shdr;

