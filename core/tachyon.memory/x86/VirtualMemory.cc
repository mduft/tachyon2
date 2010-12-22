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
#define PSTRUCT_FLAGS   0xFFF
#define TEMPMAP_PAGES   0xa
#define TEMPMAP_ADDR(x) (((~0x0 & ~(PAGE_SIZE_4K - 1)) - ((TEMPMAP_PAGES -1) * PAGE_SIZE_4K)) + (x * PAGE_SIZE_4K))
#define INVALIDATE(x)   asm volatile("invlpg (%0)" :: "r" ((x)));

extern "C" phys_addr_t x86_temp_mapspace;
extern "C" phys_addr_t x86_pg_pd;

namespace {

/**
 * contains the temporary mapped addresses.
 */
phys_addr_t* tempMappings = &x86_temp_mapspace;

/**
 * temporarily map a page into the kernels virtual address space.
 * the x86_temp_mapspace must be present in the current vspace
 * for the mapping to be accessible (it should always be).
 *
 * \param phys  the physical address to map
 * \return      the virtual address that the pysical has been mapped to
 */
uintptr_t inline pstructMap(register phys_addr_t phys) {
    register phys_addr_t physPage = (phys & ~(PAGE_SIZE_4K-1));
    register phys_addr_t physOff  = (phys & (PAGE_SIZE_4K-1));

    for(register uint32_t i = 0; i < TEMPMAP_PAGES; ++i) {
        if(tempMappings[i] & PAGE_PRESENT && (tempMappings[i] & ~(PAGE_SIZE_4K-1)) == physPage) {
            /* the physical address is already somewhere within the current
             * temporary mapping. this may be a 4K page near the only mapped
             * before, or the same (4K or 2M) page as before. */
            return TEMPMAP_ADDR(i) | physOff;
        }
    }

    for(register uint32_t i = 0; i < TEMPMAP_PAGES; ++i) {
        if(tempMappings[i] == 0) {
            tempMappings[i] = physPage | PAGE_PRESENT | PAGE_WRITABLE | PAGE_GLOBAL;
            INVALIDATE(TEMPMAP_ADDR(i));
            return TEMPMAP_ADDR(i) | physOff;
        }
    }

    KFATAL("out of page-structure page mapping space!\n");
}

/**
 * unmaps a previously pstructMap()'d virtual address, freeing it
 * for subsequent pstructMap() calls again.
 *
 * \param virt  the virtual address to free.
 */
void inline pstructUnmap(register uintptr_t virt) {
    for(register uint32_t i = 0; i < TEMPMAP_PAGES; ++i) {
        if(TEMPMAP_ADDR(i) == virt) {
            /* is this intelligent enough? idk... */
            tempMappings[i] = 0;
            INVALIDATE(virt);
            return;
        }
    }
}

/**
 * allocates physical memory suitable for any of the paging structures
 * required for virtual memory (i.e. it allocates 4K of memory, aligned
 * to 4K, and zeroes the memory region)
 *
 * \return  the physical address of the new memory region
 */
phys_addr_t inline allocatePStructAny() {
    phys_addr_t addr = PhysicalMemory::instance().allocateAligned(PSTRUCT_SIZE, PSTRUCT_SIZE);

    if(!addr) {
        KFATAL("out of physical memory for virtual memory structures!\n");
    }

    uintptr_t virt = pstructMap(addr);
    MemoryHelper::zero(reinterpret_cast<void*>(virt), PSTRUCT_SIZE);

    pstructUnmap(virt);

    KTRACE("allocated paging-structure @ %p\n", addr);

    return addr;
}

/**
 * frees a previously allocated paging structure (see allocatePStructAny).
 *
 * \param addr  the physical address of the paging-structure to free.
 */
void inline freePStructAny(phys_addr_t addr) {
    if(!addr)
        return;

    PhysicalMemory::instance().free(addr & ~(PSTRUCT_FLAGS), PSTRUCT_SIZE);
}

/**
 * takes a virtual address, and splits it into the seperate components
 * used for page translation. it then retrieves the corresponding paging
 * structures, allocating them as needed. those paging structures are
 * then mapped into the given address space, and returned using the out
 * parameters.
 *
 * \param space     the virtual address space to map into
 * \param virt      the virtual address to operate on.
 * \param out pd    will contain the virtual addr. of the mapped pd
 * \param out pt    will contain the virtual addr. of the mapped pt
 * \param forcePresent  if this is true, all paging structures have to
 *                  exist, and are not allocated (a kernel warning is
 *                  issued, and return is false).
 * \return          true, if the split was successfull, false if an error
 *                  occured.
 */
bool inline splitVirtualAndMap(vspace_t space, uintptr_t virt, uintptr_t& pd, uintptr_t& pt, bool forcePresent) {
    register uintptr_t pde   = (virt >> 22) & 0x3FF;
    register phys_addr_t* curPs;
    
    pd = pstructMap(space);
    curPs = reinterpret_cast<phys_addr_t*>(pd);

    if(!pd) {
        KERROR("cannot map pd for address space %p\n", space);
        return false;
    }

    if(!(curPs[pde] & PAGE_PRESENT)) {
        if(forcePresent) {
            KWARN("virtual address expected to be present was not (%p)\n", virt);
            return false;
        }
        curPs[pde] = allocatePStructAny() | PAGE_PRESENT | PAGE_WRITABLE;
    }

    pt = pstructMap(curPs[pde] & ~PSTRUCT_FLAGS);
    curPs = reinterpret_cast<phys_addr_t*>(pt);

    if(!pt) {
        KERROR("cannot map pt for address space %p\n", space);
        return false;
    }

    return true;
}

} // /namespace {anonymous}

bool VirtualMemory::map(vspace_t space, uintptr_t virt, phys_addr_t phys, uint32_t flags) {
    /* TODO: lock this! */

    uintptr_t pd, pt;
    if(!splitVirtualAndMap(space, virt, pd, pt, false)) {
        KFATAL("failed to split and map virtual address\n");
    }
    
    register uintptr_t pte   = (virt >> 12) & 0x3FF;
    register phys_addr_t* ppt = reinterpret_cast<phys_addr_t*>(pt);

    if(ppt[pte] & PAGE_PRESENT) {
        KFATAL("page already present for address space %p\n", space);
    }

    ppt[pte] = phys | PAGE_PRESENT | flags;

    pstructUnmap(pt);
    pstructUnmap(pd);

    KTRACE("virtual-mapped %p -> %p (flags: 0x%x)\n", virt, phys, flags | PAGE_PRESENT);

    INVALIDATE(virt);
    return true;
}

void VirtualMemory::unmap(vspace_t space, uintptr_t virt) {
    /* TODO: lock this! */

    uintptr_t pd, pt;
    if(!splitVirtualAndMap(space, virt, pd, pt, true)) {
        KFATAL("failed to split and map virtual address\n");
    }

    register uintptr_t pde   = (virt >> 22) & 0x3FF;
    register uintptr_t pte   = (virt >> 12) & 0x3FF;
    register phys_addr_t* ppd = reinterpret_cast<phys_addr_t*>(pd);
    register phys_addr_t* ppt = reinterpret_cast<phys_addr_t*>(pt);

    if(!(ppd[pde] & PAGE_PRESENT)) {
        KWARN("pd not present for virtual address %p\n", virt);
        return;
    }

    ppt[pte] = 0;
    pstructUnmap(pt);
    pstructUnmap(pd);
    INVALIDATE(virt);
}

void VirtualMemory::activateVSpace(vspace_t space) {
    asm("mov %0, %%cr3" :: "r" (space));
}

vspace_t VirtualMemory::getCurrentVSpace() {
    uintptr_t p;

    asm("mov %%cr3, %%eax" : "=a" (p));

    return static_cast<vspace_t>(p);
}

vspace_t VirtualMemory::newVSpace() {
    phys_addr_t* space = reinterpret_cast<phys_addr_t*>(allocatePStructAny());

    /* this maps the kernel address space to every newly created
     * address space, so ther kernel need not differenciate at all
     * in which address space it is running.
     * the access to the mapped pages has a DPL=0, so only the kernel
     * can access it, user space cannot. */
    space[4] = reinterpret_cast<phys_addr_t>(&x86_pg_pd) | PAGE_PRESENT | PAGE_WRITABLE;

    return reinterpret_cast<vspace_t>(space);
}

void VirtualMemory::deleteVSpace(vspace_t space) {
    /* TODO: free physical memory for paging structures (DEPTH!) */
    register phys_addr_t* pd = reinterpret_cast<phys_addr_t*>(
        pstructMap(space));

    for(register uintptr_t pde = 0; pde < 1024; ++pde) {
        if(pd[pde] & PAGE_PRESENT ) {
            /* TODO: don't free kernel PTs! */
            freePStructAny(pd[pde]);
        }
    }

    pstructUnmap(reinterpret_cast<phys_addr_t>(pd));
    freePStructAny(static_cast<phys_addr_t>(space));
}
