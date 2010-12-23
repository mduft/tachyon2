/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include "BreakPageAllocator.h"

class CoreHeap {
    BreakPageAllocator& alloc;

    static CoreHeap inst;

    CoreHeap(BreakPageAllocator& allocator)
        :   alloc(allocator) {}
    void init();
public:
    static CoreHeap& instance() { return inst; }

    void* allocate(size_t bytes);

    template<class T>
    T* allocate(size_t count) { return reinterpret_cast<T*>(allocate(sizeof(T)*count)); }

    void free(void* mem);
};
