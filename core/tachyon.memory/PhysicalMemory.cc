/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/PhysicalMemory.h>
#include <tachyon.logging/Log.h>

PhysicalMemory PhysicalMemory::inst;

// scaling for 4K pages
#define PAGE_SHIFT 12

#define SCALE_TO_INDEX(x) ((x) >> PAGE_SHIFT)
#define SCALE_TO_PHYS(x)  ((x) << PAGE_SHIFT)
#define PAGE_COUNT(x)     (((x) >> PAGE_SHIFT) + (((x) & (PAGE_SIZE_DEFAULT -1)) ? 1 : 0))

#define ALIGN_DOWN(x, a)  ((x) & ~(a-1))
#define ALIGN_UP(x, a)    ALIGN_DOWN((x) + a, a)

void PhysicalMemory::setRegion(phys_addr_t start, phys_addr_t end, bool value) {
    if(start & (PAGE_SIZE_DEFAULT-1)) {
        KFATAL("physical memory region is not page aligned!\n");
    }

    /* align down the end to the next lower page. we cannot use chunks 
     * smaller than a page size */
    if(end & (PAGE_SIZE_DEFAULT-1)) {
        KWARN("physical memory region ends in the middle of a page. rounding down.\n");

        end = ALIGN_DOWN(end, PAGE_SIZE_DEFAULT);
    }

    uint64_t length = (end - start);

    while(length -= PAGE_SIZE_DEFAULT) {
        spBitmap.set(SCALE_TO_INDEX(start), value);
        start += PAGE_SIZE_DEFAULT;
    }
}

void PhysicalMemory::available(phys_addr_t start, phys_addr_t end) {
    setRegion(start, end, false);
}

void PhysicalMemory::reserve(phys_addr_t start, phys_addr_t end) {
    setRegion(start, end, true);
}

bool PhysicalMemory::tryAllocate(phys_addr_t phys, size_t length) {
    register size_t off;

    // TODO: lock this!
    
    if(SCALE_TO_INDEX(phys) + PAGE_COUNT(length) > spBitmap.size())
        return false;

    for(off = 0; off < PAGE_COUNT(length); ++off) {
        if(spBitmap.get(SCALE_TO_INDEX(phys) + off))
            return false;
    }

    for(off = 0; off < PAGE_COUNT(length); ++off) {
        spBitmap.set(SCALE_TO_INDEX(phys) + off, true);
    }

    KINFO("ok, addr=%p, length=%d\n", phys, length);

    return true;
}

phys_addr_t PhysicalMemory::allocateAligned(size_t length, size_t align) {
    KASSERTM(align >= PAGE_SIZE_DEFAULT, 
        "alignment must be at least the physical page size");

    KASSERTM(align % PAGE_SIZE_DEFAULT == 0,
        "alignment must be a multiple of physical page size");

    /* lowest non-zero, aligned, free index */
    ssize_t index = SCALE_TO_INDEX(ALIGN_UP(SCALE_TO_PHYS(
        spBitmap.getIndexOf(false, SCALE_TO_INDEX(align))), align));

    while(index != -1 && index <= static_cast<ssize_t>(spBitmap.size())) {
        if(tryAllocate(SCALE_TO_PHYS(index), length))
            return SCALE_TO_PHYS(index);

        /* next try - get next aligned index from first next free one onwards */
        index = SCALE_TO_INDEX(ALIGN_UP(
            SCALE_TO_PHYS(spBitmap.getIndexOf(false, index + 1)), align));
    }

    KWARN("allocation failed, addr=<any>, length=%d, align=0x%x\n", length, align);
    return 0;
}

phys_addr_t PhysicalMemory::allocateFixed(phys_addr_t phys, size_t length) {
    KASSERTM(phys % PAGE_SIZE_DEFAULT == 0,
        "alignment of physical address must be a multiple of physical page size!");

    if(tryAllocate(phys, length))
        return phys;

    KWARN("allocation failed, addr=%p, length=%d, align=<fixed>\n", phys, length);
    return 0;
}

void PhysicalMemory::free(phys_addr_t phys, size_t length) {
    register size_t off;

    for(off = 0; off < PAGE_COUNT(length); ++off) {
        spBitmap.set(SCALE_TO_INDEX(phys) + off, false);
    }
}

