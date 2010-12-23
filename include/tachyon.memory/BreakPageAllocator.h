/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.logging/Log.h>
#include "VirtualZone.h"

class BreakPageAllocator {
    VirtualZone* zone;
    uintptr_t brk;
    bool large;
    uint32_t flags;
    size_t ps;
public:
    BreakPageAllocator(VirtualZone* z, bool largePages, uint32_t defFalgs)
        :   zone(z)
        ,   brk(z->getStart()) 
        ,   large(largePages)
        ,   flags(defFalgs)
        ,   ps(large ? PAGE_SIZE_LARGE : PAGE_SIZE_DEFAULT)
    { 
        if(z->used()) { 
            KFATAL("zone %p - %p already in use!\n", z->getStart(), z->getEnd());
        }
        if(flags & PAGE_LARGE && !large) {
            KFATAL("large flag may not be specified if not operating in large mode!\n");
        }
        z->used(true);
    }

    uintptr_t increase(size_t pages);
    uintptr_t decrease(size_t pages);

    uintptr_t getBase() { return zone->getStart(); }
    uintptr_t getTop() { return brk; }

    size_t getPageSize() { return ps; }
};
