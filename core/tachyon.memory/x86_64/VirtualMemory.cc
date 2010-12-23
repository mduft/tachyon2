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
#define TEMPMAP_ADDR(x) (((~0x0ull & ~(PAGE_SIZE_2M - 1)) - ((TEMPMAP_PAGES -1) * PAGE_SIZE_2M)) + (x * PAGE_SIZE_2M))
#define INVALIDATE(x)   asm volatile("invlpg (%0)" :: "r" ((x)));

extern "C" phys_addr_t x86_64_temp_mapspace;
extern "C" phys_addr_t x86_64_pg_pml4;
extern "C" phys_addr_t x86_64_pg_pdpt_high;

namespace {

/**
 * contains the temporary mapped addresses.
 */
phys_addr_t* tempMappings = &x86_64_temp_mapspace;

/**
 * temporarily map a page into the kernels virtual address space.
 * the x86_64_temp_mapspace must be present in the current vspace
 * for the mapping to be accessible (it should always be).
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
 * \param out pml4  will contain the virtual addr. of the mapped pml4
 * \param out pdpt  will contain the virtual addr. of the mapped pdpt
 * \param out pd    will contain the virtual addr. of the mapped pd
 * \param out pt    will contain the virtual addr. of the mapped pt
 * \param large     whether the virtual address lies on a large or small page.
 * \param forcePresent  if this is true, all paging structures have to
 *                  exist, and are not allocated (a kernel warning is
 *                  issued, and return is false).
 * \return          true, if the split was successfull, false if an error
 *                  occured.
 */
bool inline splitVirtualAndMap(vspace_t space, uintptr_t virt, uintptr_t& pml4, uintptr_t& pdpt, uintptr_t& pd, uintptr_t& pt, bool large, bool forcePresent) {
    register uintptr_t pml4e = (virt >> 39);
    register uintptr_t pdpte = (virt >> 30) & 0x1FF;
    register uintptr_t pde   = (virt >> 21) & 0x1FF;
    register phys_addr_t* curPs;
    
    pml4 = pstructMap(space);
    curPs = reinterpret_cast<phys_addr_t*>(pml4);

    if(!pml4) {
        KERROR("cannot map pml4 for address space %p\n", space);
        return false;
    }

    if(!(curPs[pml4e] & PAGE_PRESENT)) {
        if(forcePresent) {
            return false;
        }
        curPs[pml4e] = allocatePStructAny() | PAGE_PRESENT | PAGE_WRITABLE;
    }

    pdpt = pstructMap(curPs[pml4e] & ~PSTRUCT_FLAGS);
    curPs = reinterpret_cast<phys_addr_t*>(pdpt);

    if(!pdpt) {
        KERROR("cannot map pdpt for address space %p\n", space);
        return false;
    }

    if(!(curPs[pdpte] & PAGE_PRESENT)) {
        if(forcePresent) {
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

bool VirtualMemory::map(vspace_t space, uintptr_t virt, phys_addr_t phys, uint32_t flags) {
    /* TODO: lock this! */

    uintptr_t pml4, pdpt, pd, pt;
    if(!splitVirtualAndMap(space, virt, pml4, pdpt, pd, pt, (flags & PAGE_LARGE), false)) {
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
    pstructUnmap(pml4);

    KTRACE("virtual-mapped %p -> %p (flags: 0x%x)\n", virt, phys, flags | PAGE_PRESENT);

    INVALIDATE(virt);
    return true;
}

void VirtualMemory::unmap(vspace_t space, uintptr_t virt) {
    /* TODO: lock this! */

    uintptr_t pml4, pdpt, pd, pt;
    if(!splitVirtualAndMap(space, virt, pml4, pdpt, pd, pt, false, true)) {
        KFATAL("failed to split and map virtual address\n");
    }

    register uintptr_t pde   = (virt >> 21) & 0x1FF;
    register phys_addr_t* ppd = reinterpret_cast<phys_addr_t*>(pd);
    register phys_addr_t* ppt = reinterpret_cast<phys_addr_t*>(pt);

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
    pstructUnmap(pml4);
    INVALIDATE(virt);
}

phys_addr_t VirtualMemory::getMappedAddr(vspace_t space, uintptr_t virt) {
    uintptr_t pml4, pdpt, pd, pt;

    if(!splitVirtualAndMap(space, virt, pml4, pdpt, pd, pt, false, true))
        return 0;

    register uintptr_t pde   = (virt >> 21) & 0x1FF;
    register phys_addr_t* ppd = reinterpret_cast<phys_addr_t*>(pd);
    register phys_addr_t* ppt = reinterpret_cast<phys_addr_t*>(pt);
    register phys_addr_t mapped = 0;

    if(ppd[pde] & PAGE_LARGE) {
        mapped = ppd[pde] & ~PSTRUCT_FLAGS;
    } else {
        register uintptr_t pte   = (virt >> 12) & 0x1FF;
        mapped =  ppt[pte] & ~PSTRUCT_FLAGS;
        pstructUnmap(pt);
    }

    pstructUnmap(pd);
    pstructUnmap(pdpt);
    pstructUnmap(pml4);

    return mapped;
}

void VirtualMemory::activateVSpace(vspace_t space) {
    asm("mov %0, %%cr3" :: "r" (space));
}

vspace_t VirtualMemory::getCurrentVSpace() {
    vspace_t p;

    asm("mov %%cr3, %%rax" : "=a" (p));

    return p;
}

vspace_t VirtualMemory::newVSpace() {
    phys_addr_t* space = reinterpret_cast<phys_addr_t*>(allocatePStructAny());

    /* this maps the kernel address space to every newly created
     * address space, so ther kernel need not differenciate at all
     * in which address space it is running.
     * the access to the mapped pages has a DPL=0, so only the kernel
     * can access it, user space cannot. */
    space[511] = reinterpret_cast<phys_addr_t>(&x86_64_pg_pdpt_high) | PAGE_PRESENT | PAGE_WRITABLE;

    return reinterpret_cast<vspace_t>(space);
}

void VirtualMemory::deleteVSpace(vspace_t space) {
    /* TODO: free physical memory for paging structures (DEPTH!) */

    register phys_addr_t* pml4 = reinterpret_cast<phys_addr_t*>(pstructMap(space));
    for(register uintptr_t pml4e = 0; pml4e < 511; ++pml4e) {
        if(pml4[pml4e] & PAGE_PRESENT && (pml4[pml4e] & ~(PSTRUCT_FLAGS)) != 
                reinterpret_cast<phys_addr_t>(&x86_64_pg_pdpt_high)) {

            /* a user-level pml4 entry, so free it (recursively) */
            register phys_addr_t* pdpt = reinterpret_cast<phys_addr_t*>(
                pstructMap(pml4[pml4e]));

            for(register uintptr_t pdpte = 0; pdpte < 512; ++pdpte) {
                if(pdpt[pdpte] & PAGE_PRESENT) {
                    register phys_addr_t* pd = reinterpret_cast<phys_addr_t*>(
                        pstructMap(pdpt[pdpte]));

                    for(register uintptr_t pde = 0; pde < 512; ++pde) {
                        if(pd[pde] & PAGE_PRESENT && !(pd[pde] & PAGE_LARGE)) {
                            freePStructAny(pd[pde]);
                        }
                    }

                    pstructUnmap(reinterpret_cast<uintptr_t>(pd));
                    freePStructAny(pdpt[pdpte]);
                }
            }

            pstructUnmap(reinterpret_cast<uintptr_t>(pdpt));
            freePStructAny(pml4[pml4e]);
        }
    }
    pstructUnmap(reinterpret_cast<uintptr_t>(pml4));
    freePStructAny(space);
}
