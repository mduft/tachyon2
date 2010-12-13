/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/architecture.h>
#include <tachyon.logging/Log.h>

#include <tachyon.binary/elf/hdr.h>
#include <tachyon.binary/elf/shdr.h>
#include <tachyon.binary/elf/sym.h>

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

    static Elf_Shdr* strtab = 0;
    static Elf_Shdr* symtab = 0;

    struct syminfo info;
    info.name = defName;
    info.addr = ip;

    if(strtab == 0 || symtab == 0) {
        Elf_Ehdr* ptr = reinterpret_cast<Elf_Ehdr*>(&CORE_LMA_START);
        Elf_Shdr* shstrtab = 0;

        if(ptr->e_ident[EI_MAG1] != 'E' 
                || ptr->e_ident[EI_MAG2] != 'L' 
                || ptr->e_ident[EI_MAG3] != 'F') {
            KERROR("kernel elf header damaged!\n");
            return info;
        }

        Elf_Shdr* sptr = reinterpret_cast<Elf_Shdr*>(
            reinterpret_cast<uintptr_t>(&CORE_LMA_START) + ptr->e_shoff);

        if(sptr->sh_type != 0 || sptr->sh_size != 0) {
            // Actually, _all_ fields have to be zero for the first section!
            KERROR("first section not zeroed.\n");
            return info;
        }

        for(Elf_Half scnt = ptr->e_shnum; scnt > 0; scnt--) {
            if(sptr->sh_type == SHT_SYMTAB) {
                if(symtab != 0) {
                    KWARN("duplicate kernel symbol table!\n");
                }
                symtab = sptr;
            } else if(sptr->sh_type == SHT_STRTAB) {
                if(ptr->e_shnum - scnt == ptr->e_shstrndx) {
                    if(shstrtab != 0) {
                        KWARN("duplicate kernel section string table!\n");
                    }
                    shstrtab = sptr;
                } else {
                    if(strtab != 0) {
                        KWARN("duplicate kernel string table!\n");
                    }
                    strtab = sptr;
                }
            }

            sptr = reinterpret_cast<Elf_Shdr*>(
                reinterpret_cast<uintptr_t>(sptr) + ptr->e_shentsize);
        }

        if(symtab == 0 || strtab == 0 || shstrtab == 0) {
            KERROR("either symbol table or string table not found!\n");
            return info;
        }
    }

    Elf_Sym* sym = reinterpret_cast<Elf_Sym*>(
        reinterpret_cast<uintptr_t>(&CORE_LMA_START) + symtab->sh_offset);
    Elf_Sym* sym_start = sym;
    Elf_Sym* nearest = 0;

    while(reinterpret_cast<uintptr_t>(sym) < 
            (reinterpret_cast<uintptr_t>(sym_start) + symtab->sh_size)) {

        Elf_Word type, binding;
        STI_DECODE(sym->st_info, binding, type);

        if(sym->st_value < ip && (nearest == 0 
                || (nearest->st_value < sym->st_value))) {
            nearest = sym;
        }

        ++sym;
    }


    if(nearest != 0) {
        info.name = reinterpret_cast<char const*>(
            reinterpret_cast<uintptr_t>(&CORE_LMA_START) + strtab->sh_offset + nearest->st_name);

        info.addr = nearest->st_value;
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


