/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/VirtualMemory.h>
#include <tachyon.memory/PhysicalMemory.h>

#include <tachyon.logging/Log.h>

VirtualMemory VirtualMemory::inst;

/* ATTENTION: the following temporary mapping stuff has to be
 * synchronized with the stuff in x86_64/boot/paging.S to match.
 * (i.e. there is a temp pd that has 10 reserved PDEs for the
 * temporary mappings, and defines a pointer to the first of them). */

#define PSTRUCT_SIZE    0x1000
#define TEMPMAP_ADDR(x) (((~0x0ull) & ~(PAGE_SIZE_2M -1)) - (x * PAGE_SIZE_2M))
#define TEMPMAP_PAGES   0xa
#define INVALIDATE(x)   asm volatile("invlpg (%0)" :: "r" ((x)));

extern "C" uintptr_t x86_64_temp_mapspace;
extern "C" uintptr_t x86_64_pg_pml4;

namespace {

uintptr_t* tempMappings = &x86_64_temp_mapspace;

uintptr_t temporaryMap(register void* phys) {
    register uintptr_t physPage = (reinterpret_cast<uintptr_t>(phys) & ~(PAGE_SIZE_2M-1));
    register uintptr_t physOff  = (reinterpret_cast<uintptr_t>(phys) & (PAGE_SIZE_2M-1));

    for(register uint32_t i = 0; i < TEMPMAP_PAGES; ++i) {
        if((tempMappings[i] & ~(PAGE_SIZE_2M-1)) == physPage) {
            /* the physical address is already somewhere within the current
             * temporary mapping. this may be a 4K page near the only mapped
             * before, or the same (4K or 2M) page as before. */
            return TEMPMAP_ADDR(i) | physOff;
        }
    }

    /* ATTENTION, ATTENTION: the maps both large and small pages
     * the same way, and returns the correct location for the
     * physical memory region's start. However more memory before
     * and after the page may be mapped. */

    for(register uint32_t i = 0; i < TEMPMAP_PAGES; ++i) {
        if(tempMappings[i] == 0) {
            tempMappings[i] = physPage | PAGE_PRESENT | PAGE_LARGE;
            INVALIDATE(TEMPMAP_ADDR(i));
            return TEMPMAP_ADDR(i) | physOff;
        }
    }

    KFATAL("out of temporary page mapping space!\n");
}

void temporaryUnmap(register uintptr_t virt) {
    for(register uint32_t i = 0; i < TEMPMAP_PAGES; ++i) {
        register uintptr_t page = tempMappings[i] & ~(PAGE_SIZE_2M-1);

        if(page < virt && (page + PAGE_SIZE_2M) > virt) {
            /* is this intelligent enough? idk... */
            tempMappings[i] = 0;
            INVALIDATE(TEMPMAP_ADDR(i));
            return;
        }
    }
}

uintptr_t allocatePStructAny() {
    /* TODO: CLEAR the page to zero! */
    uintptr_t addr = PhysicalMemory::instance().allocateAligned(PSTRUCT_SIZE, PSTRUCT_SIZE);

    uintptr_t virt = temporaryMap(reinterpret_cast<void*>(addr));
    MemoryHelper::zero(reinterpret_cast<void*>(virt), PSTRUCT_SIZE);

    /* no need to unmap the temporary mapping... */
    return addr;
}

//uintptr_t* currentPd = 0;
//uintptr_t* currentPt = 0;


} // /namespace {anonymous}

uintptr_t VirtualMemory::map(vspace_t* space, uintptr_t phys, size_t count, pagesize_t ps) {
    /* find an suitable region in virtual memory, and allocate it,
     * mapping it to the given physical (consecutive!!) region. */

    /* TODO: lock this! */
    uintptr_t virt = 0;

    return virt;
}

void VirtualMemory::unmap(vspace_t* space, uintptr_t virt, size_t pages) {
    /* TODO */
}

void VirtualMemory::activateVSpace(vspace_t* space) {
    asm("mov %0, %%cr3" :: "r" (space));
}

vspace_t* VirtualMemory::getCurrentVSpace() {
    vspace_t* p;

    asm("mov %%cr3, %%rax" : "=a" (p));

    return p;
}

vspace_t* VirtualMemory::newVSpace() {
    return reinterpret_cast<vspace_t*>(allocatePStructAny());
}

