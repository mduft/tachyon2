/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

class InterruptManager {
    static InterruptManager inst;
public:
    static InterruptManager& instance() { return inst; }

    void dispatch(interrupt_t* state);
};
