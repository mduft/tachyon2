/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.processes/Scheduler.h>
#include <tachyon.collections/LinkedList.h>

Scheduler& Scheduler::instance() {
    static Scheduler inst;
    return inst;
}

Scheduler::Scheduler() 
    :   pKernel(new LinkedList<ProcessPtr>())
    ,   pHigh(new LinkedList<ProcessPtr>())
    ,   pNormal(new LinkedList<ProcessPtr>())
    ,   pLow(new LinkedList<ProcessPtr>()) {
}

SmartPointer<ProcessCollection> Scheduler::getListForPrio(Scheduler::priority_t prio) {
    switch(prio) {
    case Scheduler::Kernel : return pKernel;
    case Scheduler::High   : return pHigh;
    case Scheduler::Normal : return pNormal;
    case Scheduler::Low    : return pLow;
    }
}

void Scheduler::schedule() {
    
}

void Scheduler::addProcess(ProcessPtr proc, priority_t prio) {
    getListForPrio(prio)->add(proc);
}

