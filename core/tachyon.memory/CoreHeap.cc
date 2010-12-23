/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/VirtualZoneManager.h>
#include <tachyon.memory/CoreHeap.h>

namespace {
BreakPageAllocator coreHeapAllocator(
    VirtualZoneManager::instance().define(CORE_HEAP_START, CORE_HEAP_END), true, PAGE_WRITABLE);
} // /namespace {anonymous}

CoreHeap CoreHeap::inst(coreHeapAllocator);

typedef uintptr_t cheap_blockinfo_t;

#define CHEAP_BLOCK_ALIGN   sizeof(uintptr_t)
#define CHEAP_ALIGN_DN(x)   ((x) & ~(CHEAP_BLOCK_ALIGN-1))
#define CHEAP_ALIGN_UP(x)   CHEAP_ALIGN_DN((x) + CHEAP_BLOCK_ALIGN)
#define CHEAP_CLEAR_FLAGS(x) CHEAP_ALIGN_DN(x)
#define CHEAP_FOOTER(h)     reinterpret_cast<cheap_blockinfo_t*>(   \
                reinterpret_cast<uintptr_t>(h) + sizeof(cheap_blockinfo_t) + CHEAP_CLEAR_FLAGS(*h))
#define CHEAP_HEADER(f)     reinterpret_cast<cheap_blockinfo_t*>(   \
                reinterpret_cast<uintptr_t>(f) - sizeof(cheap_blockinfo_t) + CHEAP_CLEAR_FLAGS(*f))

/* the block info is there twice, once at the start, and once at the end... */
#define CHEAP_BLOCKSZ(x)    ((x) + (sizeof(cheap_blockinfo_t) * 2))
#define CHEAP_DATASIZE(x)   ((x) - (sizeof(cheap_blockinfo_t) * 2))
#define CHEAP_BLOCKPTR(x)   reinterpret_cast<void*>(((*x & BLOCK_FOOTER) ? \
    reinterpret_cast<uintptr_t>(x) - CHEAP_CLEAR_FLAGS(*x) : \
    reinterpret_cast<uintptr_t>(x) + sizeof(cheap_blockinfo_t)))

#define CHEAP_NEXT_BLOCK(x) reinterpret_cast<cheap_blockinfo_t*>( \
    reinterpret_cast<uintptr_t>(x) + CHEAP_BLOCKSZ(CHEAP_CLEAR_FLAGS(*x)))

#define CHEAP_HAS_NEXT(x)   ((reinterpret_cast<uintptr_t>(CHEAP_NEXT_BLOCK(x)) >= \
    alloc.getTop()) ? false : true)

#define CHEAP_BLOCK_UPDATE(p, sz, f) \
    {   *p = ((sz) | (f)); *CHEAP_FOOTER(p) = ((sz) | (f) | BLOCK_FOOTER); }
    

#define BLOCK_PRESENT   (1 << 0)
#define BLOCK_FOOTER    (1 << 1)

void CoreHeap::init()
{
    /* allocate at least the first page, so we can store the free block info */
    alloc.increase(1);

    cheap_blockinfo_t* ptr = reinterpret_cast<cheap_blockinfo_t*>(alloc.getBase());
    *ptr = CHEAP_DATASIZE(alloc.getPageSize());
}

void* CoreHeap::allocate(size_t bytes) {
    if(alloc.getBase() == alloc.getTop())
        init();

    cheap_blockinfo_t* ptr = reinterpret_cast<cheap_blockinfo_t*>(alloc.getBase());

    bytes = CHEAP_ALIGN_UP(bytes);

    while(ptr && reinterpret_cast<uintptr_t>(ptr) < alloc.getTop()) {
        if(!(*ptr & BLOCK_PRESENT) && (CHEAP_CLEAR_FLAGS(*ptr) >= CHEAP_BLOCKSZ(bytes))) {
            /* FOUND free block. */

            /* split the free block */
            register cheap_blockinfo_t* new_f = reinterpret_cast<cheap_blockinfo_t*>(
                reinterpret_cast<uintptr_t>(ptr) + CHEAP_BLOCKSZ(bytes));

            register size_t new_sz = CHEAP_CLEAR_FLAGS(*ptr) - CHEAP_BLOCKSZ(bytes);
            CHEAP_BLOCK_UPDATE(new_f, new_sz, 0);
            
            /* now write header and footer for new block */
            CHEAP_BLOCK_UPDATE(ptr, bytes, BLOCK_PRESENT);
            return CHEAP_BLOCKPTR(ptr);
        }
        if(!CHEAP_HAS_NEXT(ptr)) {
            alloc.increase(1);

            CHEAP_BLOCK_UPDATE(ptr, CHEAP_CLEAR_FLAGS(*ptr) + alloc.getPageSize(), 0);
            continue;
        }
        ptr = CHEAP_NEXT_BLOCK(ptr);
    }

    return 0;
}

void CoreHeap::free(void* mem) {
    if(!mem)
        return;

    cheap_blockinfo_t* ptr = reinterpret_cast<cheap_blockinfo_t*>(
        reinterpret_cast<uintptr_t>(mem) - sizeof(cheap_blockinfo_t));

    if(reinterpret_cast<uintptr_t>(ptr) & (CHEAP_BLOCK_ALIGN-1))
    {
        KWARN("request to free unaligned memory, ignoring\n");
        return;
    }

    if(!(*ptr & BLOCK_PRESENT)) {
        KWARN("no present block at given address found, double free()?\n");
        return;
    }

    register size_t block_sz = CHEAP_CLEAR_FLAGS(*ptr);
    register cheap_blockinfo_t* footer = CHEAP_FOOTER(ptr);

    register cheap_blockinfo_t* prev_footer = reinterpret_cast<cheap_blockinfo_t*>(
        reinterpret_cast<uintptr_t>(ptr) - sizeof(cheap_blockinfo_t));

    if(reinterpret_cast<uintptr_t>(prev_footer) > alloc.getBase() && !(*prev_footer & BLOCK_PRESENT)) {
        /* previous block is free, merge */
        register cheap_blockinfo_t* prev_hdr = CHEAP_HEADER(prev_footer);

        if(reinterpret_cast<uintptr_t>(prev_hdr) < alloc.getBase()) {
            KWARN("corrupt block footer, pointing to out-of-bounds header!\n");
            return;
        }

        CHEAP_BLOCK_UPDATE(prev_hdr, CHEAP_CLEAR_FLAGS(*prev_hdr) + block_sz, 0);

        if(CHEAP_FOOTER(prev_hdr) != footer) {
            KWARN("end of merged free block differs from original end of block!\n");
        }

        ptr = prev_hdr;
    } else {
        /* leave block as is, but update flags to not present */
        CHEAP_BLOCK_UPDATE(ptr, block_sz, 0);
    }

    register cheap_blockinfo_t* next_hdr = CHEAP_NEXT_BLOCK(ptr);

    if(reinterpret_cast<uintptr_t>(next_hdr) <= alloc.getTop() && !(*next_hdr & BLOCK_PRESENT)) {
        /* next block is free, so merge. */
        CHEAP_BLOCK_UPDATE(ptr, CHEAP_CLEAR_FLAGS(*ptr) + CHEAP_CLEAR_FLAGS(*next_hdr), 0);
    }
}
