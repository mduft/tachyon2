/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.processes/Scheduler.h>
#include <tachyon.collections/LinkedList.h>

Scheduler& Scheduler::instance() {
    static Scheduler inst;
    return inst;
}

Scheduler::Scheduler() {
    for(uint32_t i = 0; i < (Lowest + 1); ++i) {
        processes[i] = new LinkedList<ProcessPtr>();
    }
}

Thread* Scheduler::findReadyThread(Process* proc) {
    if(!proc) {
        KWARN("NULL process found!\n");
        return 0;
    }

    /* TODO: need to make iterations part of the collection interface... but how? */
    SmartPointer<ThreadCollection> threads = proc->getThreads();

    for(ThreadCollection::Iterator it = threads->iterator(); !it.end(); ++it) {
        if((*it)->isReady()) {
            return it->get();
        }
    }

    return 0;
}

void Scheduler::schedule() {
    for(uint32_t i = 0; i < (Lowest + 1); ++i) {
        for(ProcessCollection::Iterator it = processes[i]->iterator(); !it.end(); ++it) {
            Process* pr = it->get();
            Thread* thr = findReadyThread(pr);

            if(!thr)
                continue;

            KINFO("switch to thread %d for process %d\n", thr->getId(), (*it)->getId());
            thr->switchTo();
        }
    }
}

void Scheduler::addProcess(ProcessPtr proc, priority_t prio) {
    processes[prio]->add(proc);
}

