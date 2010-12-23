/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

class VirtualMemory {
    static VirtualMemory inst;

    VirtualMemory() {}

public:
    static VirtualMemory& instance() { return inst; }

    /**
     * maps a virtual region to a physical one. the physical region has
     * to be contigous, and the virtual region will be too.
     *
     * \param space the address space to operate in.
     * \param virt  the target virtual address.
     * \param phys  the start of the physical region to map.
     * \param flags one or more paging flags (PAGE_WRITABLE, ...).
     * \return      the virtual address, or 0 on error.
     */
    bool map(vspace_t space, uintptr_t virt, phys_addr_t phys, uint32_t flags);

    /**
     * unmaps a virtual address, freeing it for subsequent map() calls.
     *
     * \param space the address space to operate on.
     * \param virt  the virtual address to free. page sizes are automatically
     *              detected, and depend on how the virtual region was map()'ed
     * \param pages the number of pages to free. the pagesize is determined from
     *              the first page, and the rest is treated the same size.
     */
    void unmap(vspace_t space, uintptr_t virt);

    /**
     * returns the physical address, that the virtual address is mapped to.
     *
     * \param space the virtual address space to operate on.
     * \param virt  the virtual address to resolve.
     * \return      the mapped physical address, or 0 if not mapped.
     */
    phys_addr_t getMappedAddr(vspace_t space, uintptr_t virt);

    /**
     * activates the given address space mapping on the current CPU.
     *
     * \param space the address space to activate.
     */
    void activateVSpace(vspace_t newSpace);

    /**
     * retrieves the currently active address space for the current CPU.
     *
     * \return      the currently active address space.
     */
    vspace_t getCurrentVSpace();

    /**
     * creates a new (and empty) address space. The kernel is mapped in the high
     * address ranges of the new address space.
     *
     * \return      the virtual address space.
     */
    vspace_t newVSpace();

    /**
     * deletes an address space previously created with newVSpace(), and frees
     * all associated paging structures.
     *
     * \param space the address space to remove
     */
    void deleteVSpace(vspace_t space);
};
