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
        Kernel = 0,
        High,
        Normal,
        Low
    } priority_t;

private:
    SmartPointer<ProcessCollection> pKernel;
    SmartPointer<ProcessCollection> pHigh;
    SmartPointer<ProcessCollection> pNormal;
    SmartPointer<ProcessCollection> pLow;

    SmartPointer<ProcessCollection> getListForPrio(priority_t prio);

    Scheduler();
public:
    static Scheduler& instance();
    
    void schedule();

    void addProcess(ProcessPtr proc, priority_t prio);
    void removeProcess(ProcessPtr proc);
};

