/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

typedef struct {
    uintptr_t cr3;
} vspace_t;

class VirtualMemory {
    static VirtualMemory inst;

    VirtualMemory() {}
public:
    uintptr_t addMapping(vspace_t space, uintptr_t virt, uintptr_t phys, size_t pages);
    uintptr_t addMapping(vspace_t space, uintptr_t phys, size_t pages);
    void removeMapping(vspace_t space, uintptr_t virt, size_t pages);

    vspace_t changeAddressSpace(vspace_t newSpace);
};
