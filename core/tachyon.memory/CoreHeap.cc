/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/VirtualZoneManager.h>
#include <tachyon.memory/CoreHeap.h>

namespace {
BreakPageAllocator coreHeapAllocator(
    VirtualZoneManager::instance().define(CORE_HEAP_START, CORE_HEAP_END), true, PAGE_WRITABLE);
} // /namespace {anonymous}

CoreHeap CoreHeap::inst(coreHeapAllocator);

void* CoreHeap::allocate(size_t bytes) {
    return 0;
}
