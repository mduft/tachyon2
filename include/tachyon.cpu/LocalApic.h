/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

class LocalApic {
    static uint32_t* getBase();

public:
    static uint32_t getId();
};
