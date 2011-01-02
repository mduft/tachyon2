/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.logging/Log.h>
#include <tachyon.cpu/CpuId.h>

#define CPUID(a, b, c, d, leaf, cx) \
    asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(leaf), "c"(cx));

idleaf_t CpuId::getLeaf(uint32_t eax, uint32_t ecx) {
    idleaf_t leaf;

    CPUID(leaf.ax, leaf.bx, leaf.cx, leaf.dx, eax, ecx);
    return leaf;
}

