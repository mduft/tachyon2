/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

typedef struct {
    uint32_t ax;
    uint32_t bx;
    uint32_t cx;
    uint32_t dx;
} idleaf_t;

class CpuId {
public:
    static idleaf_t getLeaf(uint32_t eax, uint32_t ecx);
};

