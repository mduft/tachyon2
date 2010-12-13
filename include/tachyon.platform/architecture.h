/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

/* common things for C code, not C++ */
#ifndef __cplusplus
#define NULL (void*)0
#endif

/* declare interfaces using this to ease finding them.
 * Use struct so all members are public bu default. */
#define IFace struct

#include "compiler.h"

#if defined(__X86__)
# include "x86/x86.h"
# define TACHYON_ARCH "x86 (32-bits)"
#elif defined(__X86_64__)
# include "x86/x86_64.h"
# define TACHYON_ARCH "x86-64 (64-bits)"
#else
# error "unkown arch"
#endif

