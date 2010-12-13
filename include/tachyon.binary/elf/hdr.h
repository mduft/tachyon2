/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include "types.h"

#define EI_MAG0         0   // file id magic
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4   // file class
#define EI_DATA         5   // data encoding
#define EI_VERSION      6   // file version
#define EI_OSABI        7   // OS/ABI
#define EI_ABIVERSION   8   // ABI version
#define EI_NIDENT       16

#define ET_NONE     0   // no file type
#define ET_REL      1   // relocatable file
#define ET_EXEC     2   // executable file
#define ET_DYN      3   // shared library
#define ET_CORE     4   // core file

#define EM_NONE     0   // no specific machine
#define EM_M32      1   // AT&T WE 32100
#define EM_SPARC    2   // SPARC
#define EM_386      3   // Intel 80386
#define EM_68K      4   // Motorola 68000
#define EM_88K      5   // Motorola 88000
#define EM_860      7   // Intel 80860
#define EM_MIPS     8   // MIPS RS3000
#define EM_X86_64  62   // EM64T/AMD64

#define EV_NONE     0   // no version
#define EV_CURRENT  1   // v1

#define ELFCLASSNONE        0   // invalid class
#define ELFCLASS32          1   // elf32
#define ELFCLASS64          2   // elf64

#define ELFDATANONE         0   // invalid encoding
#define ELFDATA2LSB         1   // file is little endian
#define ELFDATA2MSB         2   // file is big endian

#define ELFOSABI_SYSV       0   // system V abi
#define ELFOSABI_HPUX       1   // hp-ux abi
#define ELFOSABI_STANDALONE 255 // standalone

typedef struct
{
    unsigned char   e_ident[EI_NIDENT];
    Elf_Half        e_type;
    Elf_Half        e_machine;
    Elf_Word        e_version;
    Elf_Addr        e_entry;
    Elf_Off         e_phoff;
    Elf_Off         e_shoff;
    Elf_Word        e_flags;
    Elf_Half        e_ehsize;
    Elf_Half        e_phentsize;
    Elf_Half        e_phnum;
    Elf_Half        e_shentsize;
    Elf_Half        e_shnum;
    Elf_Half        e_shstrndx;
} PACKED Elf_Ehdr;

