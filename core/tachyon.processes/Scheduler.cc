/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.processes/Scheduler.h>

Scheduler& Scheduler::instance() {
    static Scheduler inst;
    return inst;
}

void Scheduler::schedule() {

}

