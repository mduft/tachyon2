/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.processes/Thread.h>

class CpuContext {
    SmartPointer<Thread> thread;

    cpustate_t state;
public:
    CpuContext();

    void setThread(SmartPointer<Thread>& t) { thread = t; }
    SmartPointer<Thread> getThread() { return thread; }

    void setState(cpustate_t s) { state = s; }
    cpustate_t getState() { return state; }
};
