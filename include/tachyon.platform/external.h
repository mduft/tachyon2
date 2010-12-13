/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

extern "C" void NORETURN abort();
extern "C" void __kcon_write(char const*);

#define NI0(...) KFATAL("not implemented: " __VA_ARGS__);
#define NI(...)  NI0(__VA_ARGS__, 0);

