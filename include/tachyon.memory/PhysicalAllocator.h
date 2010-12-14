/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.collections/BitMap.h>

class PhysicalAllocator {
    BitMap spBitmap;
    BitMap lpBitmap;
    uint8_t spStorage[(64 * 1024)]; // 64KB is enough for roughly 2TB physical mem.
    uint8_t lpStorage[( 1 * 1024)]; //  1KB is enough for 2TB with 2MB pages.
    static PhysicalAllocator inst;

    PhysicalAllocator()
        :   spBitmap(&spStorage, sizeof(spStorage))
        ,   lpBitmap(&lpStorage, sizeof(lpStorage)) {
        spBitmap.setAll(true);
        lpBitmap.setAll(true);
    }
public:
    static PhysicalAllocator& instance() { return inst; }

    void available(uintptr_t start, uintptr_t length);
    uintptr_t allocate();
    uintptr_t allocate(bool large);
    uintptr_t allocate(uintptr_t phys);
    uintptr_t allocate(uintptr_t phys, bool large);
    void free(uintptr_t phys);
};
