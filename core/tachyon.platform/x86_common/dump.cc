/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/architecture.h>
#include <tachyon.logging/Log.h>

extern "C" void __dump_cpustate(cpustate_t* s, xcpustate_t* xs) {
    /* state is the state of either a 32 or 64 bit mode cpu.
     * xstate is the extended state of a 64 bit mode cpu or null */

    KWRITE("-+-- cpu state ----------------------------------------------------------------\n"
        " | rip=%p rfl=%p\n"
        " | rax=%p rbx=%p rcx=%p\n"
        " | rdx=%p rsi=%p rdi=%p\n"
        " | rsp=%p rbp=%p cr0=%p\n"
        " | cr2=%p cr3=%p cr4=%p\n",
        s->rip, s->rfl, s->rax, s->rbx, s->rcx, s->rdx,
        s->rsi, s->rdi, s->rsp, s->rbp, s->cr0, s->cr2,
        s->cr3, s->cr4);

    if(xs) {
        KWRITE(
            " | r8 =%p r9 =%p r10=%p\n"
            " | r11=%p r12=%p r13=%p\n"
            " | r14=%p r15=%p\n",
            xs->r8, xs->r9, xs->r10, xs->r11, 
            xs->r12, xs->r13, xs->r14, xs->r15);
    }
}

extern "C" uintptr_t CORE_LMA_START;
extern "C" uintptr_t _core_lma_end;

struct syminfo {
    char const* name;
    uintptr_t addr;
};

static struct syminfo getSymbolInfo(uintptr_t ip) {
    static char const* defName = "<unknown>";

    struct syminfo info;
    info.name = defName;
    info.addr = ip;

    return info;
}

#ifdef __X86_64__
# define BP "rbp"
#endif

#ifdef __X86__
# define BP "ebp"
#endif

extern "C" void __dump_stack() {
    register intptr_t* bp;
    register intptr_t ip;

    asm(
        "mov %%"BP", %0;"
        "call 1f;"
        "1: pop %1;"
        : "=r" (bp), "=r" (ip));

    intptr_t* lastBp = 0;
    intptr_t lastIp = 0;

    KWRITE("-+-- kernel stack -------------------------------------------------------------\n");

    while(bp && ip) {
        intmax_t frameSize = 0;

        if(lastBp) {
            frameSize = reinterpret_cast<intmax_t>(bp) - reinterpret_cast<intmax_t>(lastBp);
            /* minus ebp and return address */
            frameSize -= (sizeof(intptr_t) * 2);
        }

        /* TODO: symbol lookup */
        struct syminfo info = getSymbolInfo(ip);
        KWRITE(" | %p: %s + 0x%x (%d bytes)\n", ip, info.name, ip - info.addr, frameSize);

        lastBp = bp;
        lastIp = ip;

        ip = bp[1];
        bp = reinterpret_cast<intptr_t*>(bp[0]);
    }
}


