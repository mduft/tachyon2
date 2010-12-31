/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

class Thread {
public:
    Thread(Process* parent);

    bool start(uintptr_t start_addr);
    void kill();
};
