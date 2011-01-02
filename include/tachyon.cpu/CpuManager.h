/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.memory/SmartPointer.h>
#include <tachyon.cpu/Cpu.h>

class CpuManager {
    /* TODO: linked list of active CPUs */
    CpuManager() {}
public:
    static CpuManager& instance();

    void add(SmartPointer<Cpu> cpu);
    void remove(SmartPointer<Cpu> cpu);
};

