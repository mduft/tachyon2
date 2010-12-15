/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

/* implementation defined */
struct vspace_t;

class VirtualMemory {
    static VirtualMemory inst;

    VirtualMemory() {}

public:
    static VirtualMemory& instance() { return inst; }

    uintptr_t map(vspace_t* space, uintptr_t phys, size_t pages, pagesize_t size);
    void unmap(vspace_t* space, uintptr_t virt, size_t pages);

    void activateVSpace(vspace_t* newSpace);

    vspace_t* getCurrentVSpace();
    vspace_t* newVSpace();
};
