/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

/* Those type definitions work for ELF32 and ELF64 the same! */

typedef uintptr_t   Elf_Addr;
typedef uintptr_t   Elf_Off;
typedef uint16_t    Elf_Half;
typedef int32_t     Elf_Sword;
typedef uint32_t    Elf_Word;

typedef intptr_t    Elf_Sxword;
typedef uintptr_t   Elf_Xword;

