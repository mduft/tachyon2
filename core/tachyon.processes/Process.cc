/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.processes/Process.h>
#include <tachyon.collections/LinkedList.h>
#include <tachyon.memory/VirtualMemory.h>


Process::Process(vspace_t existing, pid_t pid)
        :   space(existing)
        ,   id(pid)
        ,   threads(new LinkedList<ThreadPtr>) {
}

Process::Process(pid_t pid)
        :   space(VirtualMemory::instance().newVSpace())
        ,   id(pid)
        ,   threads(new LinkedList<ThreadPtr>) {
}

Process::~Process() {
    VirtualMemory::instance().deleteVSpace(space);
}

void Process::addThread(ThreadPtr thread) {
    threads->add(thread);
}

void Process::removeThread(ThreadPtr thread) {
    threads->remove(thread);
}
