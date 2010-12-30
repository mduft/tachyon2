/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include "VirtualZone.h"

#define MAX_VZONES 32

class VirtualZoneManager {
    VirtualZone zones[MAX_VZONES];
public:
    static VirtualZoneManager& instance();

    VirtualZone* define(uintptr_t start, uintptr_t end);
    void remove(VirtualZone* zone);
};
