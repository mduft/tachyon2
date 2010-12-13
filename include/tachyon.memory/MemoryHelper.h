/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

class MemoryHelper {
public:
    static void copy(void* source, void* dest, size_t numBytes);
    static void move(void* source, void* dest, size_t numBytes);
    static void zero(void* dest, size_t numBytes);
    static void fill(void* dest, uint8_t byte, size_t numBytes);
};
