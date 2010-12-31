/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

class TimestampCounter {
    uint64_t startCount;
public:
    TimestampCounter() { startCount = getCurrentTicks(); }

    #ifdef __X86__
    uint64_t getCurrentTicks() { uint64_t c; asm volatile("rdtsc" : "=A"(c)); return c; }
    #elif defined(__X86_64__)
    uint64_t getCurrentTicks() { register uint64_t c asm("rax") = 0; asm volatile("rdtsc;shl $31, %%rbx;or %%rbx, %%rax;" : "=a"(c) :: "%rbx"); return c; }
    #endif

    uint64_t getTicksSinceStart() { return getCurrentTicks() - startCount; }
};
