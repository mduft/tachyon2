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

    bool map(vspace_t* space, uintptr_t virt, uintptr_t phys, uint32_t flags);
    void unmap(vspace_t* space, uintptr_t virt);

    void activateVSpace(vspace_t* newSpace);

    vspace_t* getCurrentVSpace();
    vspace_t* newVSpace();
    void deleteVSpace(vspace_t* space);
};
