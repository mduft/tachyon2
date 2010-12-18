/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

#define MAX_VZONES 32

typedef struct {
    uintptr_t start;
    uintptr_t end;
} vzone_t;

class VirtualZoneManager {
    static VirtualZoneManager inst;

    vzone_t zones[MAX_VZONES];
public:
    static VirtualZoneManager& instance() { return inst; }

    vzone_t* allocate(uintptr_t start, uintptr_t end);
    void free(vzone_t* zone);
};
