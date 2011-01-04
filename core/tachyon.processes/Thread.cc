/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.processes/Thread.h>
#include <tachyon.logging/Log.h>
#include <tachyon.memory/PhysicalMemory.h>
#include <tachyon.memory/VirtualMemory.h>

Thread::Thread(Process* par, thread_entry_t entry, tid_t id)
        :   parent(par)
        ,   id(id)
        ,   stack(0)
        ,   state() {

    phys_addr_t physStack = PhysicalMemory::instance().allocateAligned(PAGE_SIZE_DEFAULT, 0x1000);

    /* TODO: how can we reliably find a virtual address for the new stack?
     * answer: we need a specialized allocator in the "user-mode stack zone"
     */
    (void)physStack;

    state.ip = reinterpret_cast<uintptr_t>(entry);
}

void Thread::switchTo() {

}
