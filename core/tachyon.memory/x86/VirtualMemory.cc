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
#define TEMPMAP_ADDR(x) (((~0x0 & ~(PAGE_SIZE_2M - 1)) - ((TEMPMAP_PAGES -1) * PAGE_SIZE_2M)) + (x * PAGE_SIZE_2M))
#define INVALIDATE(x)   asm volatile("invlpg (%0)" :: "r" ((x)));

extern "C" phys_addr_t x86_temp_mapspace;
extern "C" phys_addr_t x86_pd;

namespace {

/**
 * contains the temporary mapped addresses.
 */
phys_addr_t* tempMappings = &x86_temp_mapspace;

/**
 * temporarily map a page into the kernels virtual address space.
 * the x86_64_temp_mapspace must be present in the current vspace
 * for the mapping to be accessible
 *
 * \param phys  the physical address to map
 * \return      the virtual address that the pysical has been mapped to
 */
uintptr_t inline pstructMap(register phys_addr_t phys) {
    register phys_addr_t physPage = (phys & ~(PAGE_SIZE_2M-1));
    register phys_addr_t physOff  = (phys & (PAGE_SIZE_2M-1));

    for(register uint32_t i = 0; i < TEMPMAP_PAGES; ++i) {
        if(tempMappings[i] & PAGE_PRESENT && (tempMappings[i] & ~(PAGE_SIZE_2M-1)) == physPage) {
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
            tempMappings[i] = physPage | PAGE_PRESENT | PAGE_LARGE | PAGE_WRITABLE | PAGE_GLOBAL;
            INVALIDATE(TEMPMAP_ADDR(i));
            return TEMPMAP_ADDR(i) | physOff;
        }
    }

    KFATAL("out of page-structure page mapping space!\n");
}

/**
 * unmaps a previously pstructMap()'d virtual address, freeing it
 * for subsequent pstructMap() calls again.
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

void inline freePStructAny(phys_addr_t addr) {
    if(!addr)
        return;

    PhysicalMemory::instance().free(addr & ~(PSTRUCT_FLAGS), PSTRUCT_SIZE);
}

bool inline splitVirtualAndMap(vspace_t space, uintptr_t virt, uintptr_t& pdpt, uintptr_t& pd, uintptr_t& pt, bool large, bool forcePresent) {
    register uintptr_t pdpte = (virt >> 30);
    register uintptr_t pde   = (virt >> 21) & 0x1FF;
    register phys_addr_t* curPs;
    
    pdpt = pstructMap(space);
    curPs = reinterpret_cast<phys_addr_t*>(pdpt);

    if(!pdpt) {
        KERROR("cannot map pdpt for address space %p\n", space);
        return false;
    }

    if(!(curPs[pdpte] & PAGE_PRESENT)) {
        if(forcePresent) {
            KWARN("virtual address expected to be present was not (%p)\n", virt);
            return false;
        }
        curPs[pdpte] = allocatePStructAny() | PAGE_PRESENT | PAGE_WRITABLE;
    }

    pd = pstructMap(curPs[pdpte] & ~PSTRUCT_FLAGS);
    curPs = reinterpret_cast<phys_addr_t*>(pd);

    if(!pd) {
        KERROR("cannot map pd for address space %p\n", space);
        return false;
    }

    if(forcePresent) {
        /* ATTENTION: large is overridden by the PDE in case we force
         * presence of to virtual address */
        large = (curPs[pde] & PAGE_PRESENT && curPs[pde] & PAGE_LARGE);
    }

    if(!large) {
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
    }

    return true;
}

} // /namespace {anonymous}

/**
 * maps a virtual region to a physical one. the physical region has
 * to be contigous, and the virtual region will be too.
 *
 * \param space the address space to operate in.
 * \param phys  the start of the physical region to map.
 * \param virt  the target virtual address.
 * \param ps    the size of each page (4K/2M).
 * \param writable  whether the page should be writable.
 * \return      the virtual address, or 0 on error.
 */
bool VirtualMemory::map(vspace_t space, uintptr_t virt, phys_addr_t phys, uint32_t flags) {
    /* TODO: lock this! */

    uintptr_t pdpt, pd, pt;
    if(!splitVirtualAndMap(space, virt, pdpt, pd, pt, (flags & PAGE_LARGE), false)) {
        KFATAL("failed to split and map virtual address\n");
    }
    
    register uintptr_t pde   = (virt >> 21) & 0x1FF;

    register phys_addr_t* ppd = reinterpret_cast<phys_addr_t*>(pd);
    register phys_addr_t* ppt = reinterpret_cast<phys_addr_t*>(pt);

    if(flags & PAGE_LARGE) {
        if(ppd[pde] & PAGE_PRESENT) {
            KFATAL("page already present for address space %p\n", space);
        }

        ppd[pde] = phys | PAGE_PRESENT | flags;
    } else {
        register uintptr_t pte   = (virt >> 12) & 0x1FF;

        if(ppd[pde] & PAGE_LARGE) {
            KFATAL("large page present at location for small page in address space %p\n", space);
        }

        if(ppt[pte] & PAGE_PRESENT) {
            KFATAL("page already present for address space %p\n", space);
        }

        ppt[pte] = phys | PAGE_PRESENT | flags;

        pstructUnmap(pt);
    }

    pstructUnmap(pd);
    pstructUnmap(pdpt);

    KTRACE("virtual-mapped %p -> %p (flags: 0x%x)\n", virt, phys, flags | PAGE_PRESENT);

    INVALIDATE(virt);
    return true;
}

/**
 * unmaps a virtual address, freeing it for subsequent map() calls.
 *
 * \param space the address space to operate on.
 * \param virt  the virtual address to free. page sizes are automatically
 *              detected, and depend on how the virtual region was map()'ed
 * \param pages the number of pages to free. the pagesize is determined from
 *              the first page, and the rest is treated the same size.
 */
void VirtualMemory::unmap(vspace_t space, uintptr_t virt) {
    /* TODO: lock this! */

    uintptr_t pdpt, pd, pt;
    if(!splitVirtualAndMap(space, virt, pdpt, pd, pt, false, true)) {
        KFATAL("failed to split and map virtual address\n");
    }

    register uintptr_t pde   = (virt >> 21) & 0x1FF;
    register phys_addr_t* ppd = reinterpret_cast<phys_addr_t*>(pd);
    register phys_addr_t* ppt = reinterpret_cast<phys_addr_t*>(pt);

    if(!(ppd[pde] & PAGE_PRESENT)) {
        KWARN("pd not present for virtual address %p\n", virt);
        return;
    }

    if(ppd[pde] & PAGE_LARGE) {
        register uintptr_t pte   = (virt >> 12) & 0x1FF;

        if(ppd[pde] & PAGE_LARGE) {
            KWARN("pd contains a large page, not a pt for virtual address %p\n", virt);
            return;
        }

        ppt[pte] = 0;
        pstructUnmap(pt);
    } else {
        ppd[pde] = 0;
    }

    pstructUnmap(pd);
    pstructUnmap(pdpt);
    INVALIDATE(virt);
}

/**
 * activates the given address space mapping on the current CPU.
 *
 * \param space the address space to activate.
 */
void VirtualMemory::activateVSpace(vspace_t space) {
    asm("mov %0, %%cr3" :: "r" (space));
}

/**
 * retrieves the currently active address space for the current CPU.
 *
 * \return      the currently active address space.
 */
vspace_t VirtualMemory::getCurrentVSpace() {
    uintptr_t p;

    asm("mov %%cr3, %%eax" : "=a" (p));

    return static_cast<vspace_t>(p);
}

/**
 * creates a new (and empty) address space. The kernel is mapped in the high
 * address ranges of the new address space. This wastes some memory (it
 * allocates a full PDPT as with the 64 bit paging for simplicity). It would
 * be possible to save a few KiBs (~3-4) per process here.
 *
 * \return      the virtual address space.
 */
vspace_t VirtualMemory::newVSpace() {
    phys_addr_t* space = reinterpret_cast<phys_addr_t*>(allocatePStructAny());

    /* this maps the kernel address space to every newly created
     * address space, so ther kernel need not differenciate at all
     * in which address space it is running.
     * the access to the mapped pages has a DPL=0, so only the kernel
     * can access it, user space cannot. */
    space[4] = reinterpret_cast<phys_addr_t>(&x86_pd) | PAGE_PRESENT | PAGE_WRITABLE;

    return reinterpret_cast<vspace_t>(space);
}

void VirtualMemory::deleteVSpace(vspace_t space) {
    /* TODO: free physical memory for paging structures (DEPTH!) */
    register phys_addr_t* pdpt = reinterpret_cast<phys_addr_t*>(pstructMap(space));
    for(register uintptr_t pdpte = 0; pdpte < 4; ++pdpte) {
        if(pdpt[pdpte] & PAGE_PRESENT && pdpt[pdpte] != reinterpret_cast<phys_addr_t>(&x86_pd)) {
            register phys_addr_t* pd = reinterpret_cast<phys_addr_t*>(
                pstructMap(pdpt[pdpte]));

            for(register uintptr_t pde = 0; pde < 512; ++pde) {
                if(pd[pde] & PAGE_PRESENT && !(pd[pde] & PAGE_LARGE)) {
                    freePStructAny(pd[pde]);
                }
            }

            pstructUnmap(reinterpret_cast<phys_addr_t>(pd));
            freePStructAny(pdpt[pdpte]);
        }
    }

    pstructUnmap(reinterpret_cast<phys_addr_t>(pdpt));
    freePStructAny(reinterpret_cast<phys_addr_t>(pdpt));
}
