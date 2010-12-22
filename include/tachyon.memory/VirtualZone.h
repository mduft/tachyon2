/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

class VirtualZone {
    friend class VirtualZoneManager;

    uintptr_t start;
    uintptr_t end;

    void init(uintptr_t s, uintptr_t e) { start = s; end = e; }
    void destroy() { start = 0; end = 0; }
public:
    uintptr_t getStart() { return start; }
    uintptr_t getEnd() { return end; }
};
