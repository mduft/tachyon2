/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

/* compiler dependant things. */
typedef __builtin_va_list va_list;
#define va_start(v, l)      __builtin_va_start(v,l)
#define va_end(v)           __builtin_va_end(v)
#define va_arg(v, l)        __builtin_va_arg(v,l)

#define PACKED              __attribute__((__packed__))
#define NORETURN            __attribute__((__noreturn__))
