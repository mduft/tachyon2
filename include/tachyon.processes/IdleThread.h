/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.logging/Log.h>

class IdleThread {
public:
    static void idle() { KTRACE("idling ...\n");  asm("hlt;"); }
};
