/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.memory/SmartPointer.h>
#include <tachyon.processes/Process.h>

typedef SmartPointer<Process> ProcessPtr;
typedef Collection<ProcessPtr> ProcessCollection;

class Scheduler {
    SmartPointer<ProcessCollection> processes[4];

    Scheduler();
public:
    typedef enum {
        Kernel,
        High,
        Normal,
        Low
    } priority_t;

    static Scheduler& instance();
    
    void schedule();

    void addProcess(ProcessPtr proc, priority_t prio);
    void removeProcess(ProcessPtr proc);
};

