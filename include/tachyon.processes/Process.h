/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.memory/SmartPointer.h>
#include <tachyon.processes/Thread.h>

#include <tachyon.core.iface/Collection.h>

typedef SmartPointer<Thread> ThreadPtr;
typedef Collection<ThreadPtr> ThreadCollection;

class Process {
    vspace_t space;
    pid_t id;

    SmartPointer<ThreadCollection> threads;

    /* ATTENTION: the sole purpose of this constructor is to
     * create the initial "core" process for the kernel itself.
     * Thus it doesn't matter that the destructor would destroy
     * the existing address space, as the kernel would have left
     * the "boot()" function if the destructor is ever called
     * (which is not allowed, the last statement in "boot()"
     * panics the kernel)
     */
    friend Process* createCoreProcess(pid_t pid);
    Process(vspace_t existing, pid_t pid);
public:
    Process(pid_t pid);
    ~Process();

    void addThread(ThreadPtr thread);
    void removeThread(ThreadPtr thread);

    SmartPointer<ThreadCollection> getThreads() { return threads; }

    pid_t getId() { return id; }
};
