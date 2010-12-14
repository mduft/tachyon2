/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/PhysicalAllocator.h>
#include <tachyon.logging/Log.h>

PhysicalAllocator PhysicalAllocator::inst;

#ifdef __X86_64__
# define LARGE_PAGE_SHIFT 21
# define SMALL_PAGE_SHIFT 12
#else
# define LARGE_PAGE_SHIFT 22
# define SMALL_PAGE_SHIFT 12
#endif

// scaling for 4K pages
#define SCALE_TO_INDEX_S(x) ((x) >> SMALL_PAGE_SHIFT)
#define SCALE_TO_PHYS_S(x)  ((x) << SMALL_PAGE_SHIFT)

// scaling for large pages (2MB for 64bits, 4MB for 32bits)
#define SCALE_TO_INDEX_L(x) ((x) >> LARGE_PAGE_SHIFT)
#define SCALE_TO_PHYS_L(x)  ((x) << LARGE_PAGE_SHIFT)

#define SMALL2LARGE_RELATION ((1 << LARGE_PAGE_SHIFT) / (1 << SMALL_PAGE_SHIFT))

void PhysicalAllocator::setRegion(uintptr_t start, uintptr_t length, bool value) {
    if(start & (PAGE_SIZE_DEFAULT-1) || (start + length) & (PAGE_SIZE_DEFAULT-1)) {
        KFATAL("physical memory region is not page aligned!\n");
    }

    while(length -= PAGE_SIZE_DEFAULT) {
        spBitmap.set(SCALE_TO_INDEX_S(start), value);

        if(length + PAGE_SIZE_DEFAULT >= (1 << LARGE_PAGE_SHIFT)
                && (start & ((1 << LARGE_PAGE_SHIFT) -1))) {
            lpBitmap.set(SCALE_TO_INDEX_L(start), value);
        }

        start += PAGE_SIZE_DEFAULT;
    }
}

void PhysicalAllocator::available(uintptr_t start, uintptr_t length) {
    setRegion(start, length, false);
}

void PhysicalAllocator::reserve(uintptr_t start, uintptr_t length) {
    setRegion(start, length, true);
}

uintptr_t PhysicalAllocator::allocate() {
    ssize_t index = spBitmap.getIndexOf(false);

    if(index == -1)
        return 0;

    KTRACE("got %p\n", SCALE_TO_PHYS_S(index));

    spBitmap.set(index, true);
    return SCALE_TO_PHYS_S(index);
}

uintptr_t PhysicalAllocator::allocate(bool large) {
    if(!large)
        return allocate();

    /* start at first non zero large page */
    ssize_t index = SCALE_TO_INDEX_S((1 << LARGE_PAGE_SHIFT)-1);
    while(true) {
        index = spBitmap.getIndexOf(false, index + 1);
        uintptr_t i;

        if(index == -1)
            return 0;

        if(SCALE_TO_PHYS_S(index) & ((1 << LARGE_PAGE_SHIFT)-1)) {
            /* round up to next large page */
            index = SCALE_TO_INDEX_S(((SCALE_TO_PHYS_S(index) + (1 << LARGE_PAGE_SHIFT))) & ~((1 << LARGE_PAGE_SHIFT) -1)) -1;
            continue;
        }

        for(i = 1; i < SMALL2LARGE_RELATION; ++i) {
            if(spBitmap.get(index + i))
                break;
        }

        if(i < SMALL2LARGE_RELATION) {
            index += i;

            /* round up to next large page */
            index = SCALE_TO_INDEX_S(((SCALE_TO_PHYS_S(index) + (1 << LARGE_PAGE_SHIFT))) & ~((1 << LARGE_PAGE_SHIFT) -1)) -1;
            continue;
        }

        for(i = 0; i < SMALL2LARGE_RELATION; ++i) {
            spBitmap.set(index + i, true);
        }

        register uintptr_t phys = SCALE_TO_PHYS_S(index);

        lpBitmap.set(SCALE_TO_INDEX_L(phys), true);

        KTRACE("got %p\n", SCALE_TO_PHYS_S(index));

        return phys;
    }
}

uintptr_t PhysicalAllocator::allocate(uintptr_t phys) {
    if(spBitmap.get(SCALE_TO_INDEX_S(phys)))
        return 0;

    spBitmap.set(SCALE_TO_INDEX_S(phys), true);
    KTRACE("got (fixed) %p\n", phys);
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
    KTRACE("got (fixed) %p\n", phys);
    return phys;
}
