/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/PhysicalAllocator.h>
#include <tachyon.logging/Log.h>

PhysicalAllocator PhysicalAllocator::inst;

#ifdef __X86_64__
# define LARGE_PAGE_SHIFT 20
# define SMALL_PAGE_SHIFT 12
#else
# define LARGE_PAGE_SHIFT 21
# define SMALL_PAGE_SHIFT 12
#endif

// scaling for 4K pages
#define SCALE_TO_INDEX_S(x) ((x) >> SMALL_PAGE_SHIFT)
#define SCALE_TO_PHYS_S(x)  ((x) << SMALL_PAGE_SHIFT)

// scaling for large pages (2MB for 64bits, 4MB for 32bits)
#define SCALE_TO_INDEX_L(x) ((x) >> LARGE_PAGE_SHIFT)
#define SCALE_TO_PHYS_L(x)  ((x) << LARGE_PAGE_SHIFT)

#define SMALL2LARGE_RELATION ((1 << LARGE_PAGE_SHIFT) / (1 << SMALL_PAGE_SHIFT))

void PhysicalAllocator::available(uintptr_t start, uintptr_t length) {
    if(start & PAGE_SIZE_DEFAULT || (start + length) & PAGE_SIZE_DEFAULT) {
        KFATAL("new physical memory reagion is not page aligned!");
    }

    while(length -= PAGE_SIZE_DEFAULT) {
        spBitmap.set(SCALE_TO_INDEX_S(start), true);

        if(length + PAGE_SIZE_DEFAULT >= (1 << LARGE_PAGE_SHIFT)) {
            lpBitmap.set(SCALE_TO_INDEX_L(start), true);
        }

        start += PAGE_SIZE_DEFAULT;
    }
}

uintptr_t PhysicalAllocator::allocate() {
    uintptr_t index = spBitmap.getIndexOf(false);

    spBitmap.set(index, true);
    return SCALE_TO_PHYS_S(index);
}

uintptr_t PhysicalAllocator::allocate(bool large) {
    if(!large)
        return allocate();

    ssize_t index;
    while(true) {
        index = spBitmap.getIndexOf(false, index);
        uintptr_t i;

        if(index == -1)
            return 0;

        if(index & (1 << LARGE_PAGE_SHIFT))
            continue;

        for(i = 1; i < SMALL2LARGE_RELATION; ++i) {
            if(spBitmap.get(index + i))
                break;
        }

        if(i < SMALL2LARGE_RELATION)
            continue;

        for(i = 0; i < SMALL2LARGE_RELATION; ++i) {
            spBitmap.set(index + i, true);
        }

        register uintptr_t phys = SCALE_TO_PHYS_S(index);

        lpBitmap.set(SCALE_TO_INDEX_L(phys), true);
        return phys;
    }
}

uintptr_t PhysicalAllocator::allocate(uintptr_t phys) {
    if(spBitmap.get(SCALE_TO_INDEX_S(phys)))
        return 0;

    spBitmap.set(SCALE_TO_INDEX_S(phys), true);
    return phys;
}

uintptr_t PhysicalAllocator::allocate(uintptr_t phys, bool large) {
    if(!large)
        return allocate(phys);

    if(lpBitmap.get(SCALE_TO_INDEX_L(phys)))
        return 0;

    uintptr_t idx = SCALE_TO_INDEX_S(phys);
    uintptr_t i;

    for(i = 0; i < SMALL2LARGE_RELATION; ++i) {
        if(spBitmap.get(idx + i))
            return 0;
    }

    for(i = 0; i < SMALL2LARGE_RELATION; ++i) {
        spBitmap.set(idx + i, true);
    }

    lpBitmap.set(SCALE_TO_INDEX_L(phys), true);
    return phys;
}
