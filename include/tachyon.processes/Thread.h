/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

#include <tachyon.memory/SmartPointer.h>

/* forwar declare to avoid cycle. This is also the reason why
 * raw pointers are used, instead of SmartPointers. */
class Process;

class Thread {
public:
    typedef void (*thread_entry_t)();

private:
    Process* parent;
    tid_t id;

    uintptr_t stack;
    cpustate_t state;
public:
    Thread(Process* par, thread_entry_t entry, tid_t id);

    void switchTo();
    bool isReady() { return true; }
    tid_t getId() { return id; }

    bool operator==(const Thread& other) const { return ((parent == other.parent) && (id == other.id)); }
};
