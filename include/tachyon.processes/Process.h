/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.memory/VirtualMemory.h>

class Process {
    vspace_t space;
public:
    Process()
        :   space(VirtualMemory::instance().newVSpace()) {}

    ~Process() {
        VirtualMemory::instance().deleteVSpace(space);
    }
};
