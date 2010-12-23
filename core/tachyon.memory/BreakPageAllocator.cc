/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/BreakPageAllocator.h>
#include <tachyon.memory/PhysicalMemory.h>
#include <tachyon.memory/VirtualMemory.h>

uintptr_t BreakPageAllocator::increase(size_t pages) {
    register size_t i = pages;

    if(brk + (pages * ps) > zone->getEnd()) {
        KWARN("out of virtual memory!\n");
        return 0;
    }

    while(i--) {
        phys_addr_t phys = PhysicalMemory::instance().allocateAligned(ps, ps);
        if(!VirtualMemory::instance().map(VirtualMemory::instance().getCurrentVSpace(),
                brk, phys, (large ? PAGE_LARGE : 0) | flags)) {
            /* free already allocated ones again */
            decrease(pages - (i + 1));
            return 0;
        }

        brk += ps;
    }

    return brk;
}

uintptr_t BreakPageAllocator::decrease(size_t pages) {
    if((brk - (pages * ps)) < zone->getStart()) {
        KWARN("attempt to free more break space then allocated!\n");

        pages = (brk - zone->getStart()) / ps;
    }

    while(pages--) {
        phys_addr_t phys = VirtualMemory::instance().getMappedAddr(
            VirtualMemory::instance().getCurrentVSpace(), brk - ps);

        VirtualMemory::instance().unmap(VirtualMemory::instance().getCurrentVSpace(), brk - ps);
        PhysicalMemory::instance().free(phys, ps);

        brk -= ps;
    }

    return brk;
}
