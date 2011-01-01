/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.collections/BitMap.h>

class PhysicalMemory {
    BitMap spBitmap;
    uint8_t spStorage[(1024 * 1024)]; // 1MB = 32GB physical memory.
    static PhysicalMemory inst;

    PhysicalMemory()
        :   spBitmap(&spStorage, sizeof(spStorage)) {
        spBitmap.setAll(true);
    }

    void setRegion(phys_addr_t start, phys_addr_t end, bool value);
    bool tryAllocate(phys_addr_t phys, size_t length);
public:
    static PhysicalMemory& instance() { return inst; }

    void available(phys_addr_t start, phys_addr_t end);
    void reserve(phys_addr_t start, phys_addr_t end);

    phys_addr_t allocateAligned(size_t length, size_t align);
    phys_addr_t allocateFixed(phys_addr_t phys, size_t length);
    void free(phys_addr_t phys, size_t length);
};
