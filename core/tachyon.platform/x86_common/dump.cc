/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/architecture.h>
#include <tachyon.logging/Log.h>

#include <tachyon.binary/elf/shdr.h>

extern "C" void __dump_cpustate(cpustate_t* s) {
    uintptr_t cr0 = 0, cr2 = 0, cr3 = 0, cr4 = 0;

    asm volatile("mov %%cr0, %0; mov %%cr2, %1; mov %%cr3, %2; mov %%cr4, %3"
        : "=r"(cr0), "=r"(cr2), "=r"(cr3), "=r"(cr4));

    #ifdef __X86_64__
    KWRITE("-+-- cpu state ----------------------------------------------------------------\n"
        " |  ip=%p  fl=%p\n" " |  ax=%p  bx=%p  cx=%p\n" " |  dx=%p  si=%p  di=%p\n" " |  sp=%p  bp=%p cr0=%p\n"
        " | cr2=%p cr3=%p cr4=%p\n", s->ip, s->flags, s->ax, s->bx, s->cx, s->dx,
        s->si, s->di, s->sp, s->bp, cr0, cr2, cr3, cr4);

    KWRITE(" |  r8=%p  r9=%p r10=%p\n" " | r11=%p r12=%p r13=%p\n" " | r14=%p r15=%p\n",
        s->r8, s->r9, s->r10, s->r11, s->r12, s->r13, s->r14, s->r15);
    #else
    KWRITE("-+-- cpu state ----------------------------------------------------------------\n"
        " |  ip=%p  fl=%p\n |  ax=%p  bx=%p  cx=%p  dx=%p\n |  si=%p  di=%p  sp=%p  bp=%p\n"
        " | cr0=%p cr2=%p cr3=%p cr4=%p\n", s->ip, s->flags, s->ax, s->bx, s->cx, s->dx,
        s->si, s->di, s->sp, s->bp, cr0, cr2, cr3, cr4);
    #endif
    KWRITE(" |  es=0x%2x  cs=0x%2x  ss=0x%2x  ds=0x%2x  fs=0x%2x  gs=0x%2x\n",
        s->es, s->cs, s->ss, s->ds, s->fs, s->gs);
}

extern "C" uintptr_t CORE_LMA_START;
extern "C" uintptr_t _core_lma_end;

struct syminfo {
    char const* name;
    uintptr_t addr;
};

static Elf_Shdr* _strtab;
static Elf_Shdr* _symtab;

extern "C" void __dump_set_symtab(uint32_t num, uint32_t size, uint32_t addr, uint32_t shndx) {
    KTRACE("num: %d, size: %d, addr: %p, shndx: %d\n", num, size, addr, shndx);

    _strtab=0;
    _symtab=0;

    if(size != sizeof(Elf_Shdr)) {
        KWARN("size of section headers unexpected!\n");
        return;
    }

    /* TODO: find symtab and strtab for it... */
}

static struct syminfo getSymbolInfo(uintptr_t ip) {
    static char const* defName = "<unknown>";

    struct syminfo info;
    info.name = defName;
    info.addr = ip;

    if(_symtab && _strtab) {
        /* TODO: implement as soon as the above __dump_set_symtab is implemented. */
    }

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

