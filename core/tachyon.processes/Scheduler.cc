/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.processes/Scheduler.h>
#include <tachyon.collections/LinkedList.h>

Scheduler& Scheduler::instance() {
    static Scheduler inst;
    return inst;
}

Scheduler::Scheduler() {
    for(uint32_t i = 0; i < (sizeof(processes) / sizeof(processes[0])); ++i) {
        processes[i] = new LinkedList<ProcessPtr>();
    }
}

void Scheduler::schedule() {
    
}

void Scheduler::addProcess(ProcessPtr proc, priority_t prio) {
    processes[static_cast<uint32_t>(prio)]->add(proc);
}

