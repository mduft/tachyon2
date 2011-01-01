/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.core.iface/Collection.h>
#include <tachyon.logging/Log.h>

typedef void (*interrupthandler_t)(interrupt_t* state);

class InterruptManager {
    static InterruptManager inst;
    
    /* currently, the IDT for all platforms, specifies
     * "only" 60 ISRs, so we need to handle only those
     * 60, not the full 255 */
    interrupthandler_t handlers[60];
public:
    static InterruptManager& instance() { return inst; }

    void dispatch(interrupt_t* state);

    void addHandler(uint8_t vector, interrupthandler_t handler) {
        if(handlers[vector] != 0) {
            KWARN("multiple idt handlers not implemented yet, replacing vector %d!\n", vector);
        }

        handlers[vector] = handler;
    }

    void removeHandler(uint8_t vector) { handlers[vector] = 0; }
};
