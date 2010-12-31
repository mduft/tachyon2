/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

#include <tachyon.cpu/CpuContext.h>
#include <tachyon.memory/SmartPointer.h>

class Cpu {
    uint32_t id;
    SmartPointer<CpuContext> context;

public:
    Cpu(uint32_t cpuId)
        :   id(cpuId) {}

    void setCurrentContext(SmartPointer<CpuContext>& c);
};
