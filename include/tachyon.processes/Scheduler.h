/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.memory/SmartPointer.h>
#include <tachyon.processes/Process.h>

typedef SmartPointer<Process> ProcessPtr;
typedef Collection<ProcessPtr> ProcessCollection;

class Scheduler {
public:
    typedef enum {
        Highest = 0,
        High    = 1,
        Normal  = 2,
        Low     = 3,
        Lowest  = 4,
    } priority_t;
private:
    ProcessCollection* processes[Lowest + 1];

    Thread* findReadyThread(Process* proc);

    Scheduler();
public:
    static Scheduler& instance();
    
    void schedule();

    void addProcess(ProcessPtr proc, priority_t prio);
    void removeProcess(ProcessPtr proc);
};

