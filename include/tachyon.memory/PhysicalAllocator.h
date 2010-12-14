/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.collections/BitMap.h>

class PhysicalAllocator {
    BitMap spBitmap;
    uint8_t spStorage[(64 * 1024)]; // 64KB is enough for roughly 2TB physical mem.
    static PhysicalAllocator inst;

    PhysicalAllocator()
        :   spBitmap(&spStorage, sizeof(spStorage)) {
        spBitmap.setAll(true);
    }

    void setRegion(uintptr_t start, uintptr_t length, bool value);
    bool tryAllocate(uintptr_t phys, size_t length);
public:
    static PhysicalAllocator& instance() { return inst; }

    void available(uintptr_t start, uintptr_t length);
    void reserve(uintptr_t start, uintptr_t length);

    uintptr_t allocateAligned(size_t length, size_t align);
    uintptr_t allocateFixed(uintptr_t phys, size_t length);
    void free(uintptr_t phys, size_t length);
};
