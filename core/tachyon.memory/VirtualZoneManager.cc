/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/VirtualZoneManager.h>
#include <tachyon.logging/Log.h>

VirtualZoneManager& VirtualZoneManager::instance() {
    static VirtualZoneManager inst;
    return inst; 
}

VirtualZone* VirtualZoneManager::define(uintptr_t start, uintptr_t end) {
    VirtualZone* freeZone = 0;
    for(register uintptr_t i = 0; i < MAX_VZONES; ++i) {
        if(zones[i].start == 0 && zones[i].end == 0) {
            freeZone = &zones[i];
        } else {
            if((zones[i].start >= start && zones[i].start < end) ||
                    (zones[i].end > start && zones[i].end <= end)) {
                KTRACE("virtual memory zone already allocated"
                    "old: {%p, %p}, new: {%p, %p}\n", zones[i].start,
                    zones[i].end, start, end);
                return 0;
            }
        }
    }

    if(!freeZone)
        return 0;

    freeZone->init(start, end);

    return freeZone;
}

void VirtualZoneManager::remove(VirtualZone* zone) {
    zone->destroy();
}
