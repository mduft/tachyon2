/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/VirtualZoneManager.h>
#include <tachyon.logging/Log.h>

VirtualZoneManager VirtualZoneManager::inst;

vzone_t* VirtualZoneManager::allocate(uintptr_t start, uintptr_t end) {
    vzone_t* freeZone = 0;
    for(register uintptr_t i = 0; i < MAX_VZONES; ++i) {
        if(zones[i].start == 0 && zones[i].end == 0) {
            freeZone = &zones[i];
        } else {
            if(zones[i].start <= start && end >= end) {
                KTRACE("virtual memory zone already allocated"
                    "old: {%p, %p}, new: {%p, %p}\n", zones[i].start,
                    zones[i].end, start, end);
                return 0;
            }
        }
    }

    if(!freeZone)
        return 0;

    freeZone->start = start;
    freeZone->end = end;

    return freeZone;
}

void free(vzone_t* zone) {
    zone->start = 0;
    zone->end = 0;
}
