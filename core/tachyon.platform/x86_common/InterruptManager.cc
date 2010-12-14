/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/x86/InterruptManager.h>
#include <tachyon.logging/Log.h>

extern "C" void interrupt_l0(interrupt_t * state) {
    KFATAL("unhandled interrupt 0x%x at %p (0x%x)\n", state->interrupt, state->ip, state->code);
}

