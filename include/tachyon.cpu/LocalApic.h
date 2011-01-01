/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

class LocalApic {
public:
    static uintptr_t getPhysicalBase();
    static uint32_t getId();
    static bool isPrimaryCpu();
    static bool isEnabled();

    static void init();
    static void eoi();
};
