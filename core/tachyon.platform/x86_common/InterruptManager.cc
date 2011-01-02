/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/x86/InterruptManager.h>
#include <tachyon.logging/Log.h>
#include <tachyon.collections/LinkedList.h>
#include <tachyon.cpu/LocalApic.h>

InterruptManager InterruptManager::inst;

extern "C" void interrupt_l0(interrupt_t * state) {
    InterruptManager::instance().dispatch(state);
}

void InterruptManager::dispatch(interrupt_t* state) {
    if(handlers[state->interrupt] != 0) {
        handlers[state->interrupt](state);
    } else {
        KWARN("unhandled interrupt 0x%x at %p (0x%x)\n", state->interrupt, state->ip, state->code);
    }

    LocalApic::eoi();
}
