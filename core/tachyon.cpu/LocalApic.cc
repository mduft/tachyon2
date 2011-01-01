/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.logging/Log.h>
#include <tachyon.cpu/LocalApic.h>

#if defined(__X86__)
# define RD_APIC_MSR(x) asm volatile("rdmsr" : "=A"(x) : "c"(IA32_APIC_BASE));
#elif defined (__X86_64__)
# define RD_APIC_MSR(x) asm volatile("xor %%rax, %%rax; rdmsr; shl $31, %%rbx; or %%rbx, %%rax;" : "=a"(x) : "c"(IA32_APIC_BASE) : "%rbx");
#endif

typedef struct {

} lapic_t;

uint32_t LocalApic::getId() {
    uint64_t msr;
    lapic_t* base;

    RD_APIC_MSR(msr);
    base = reinterpret_cast<lapic_t*>(msr & 0xFFFFF000);

    KINFO("APIC base at %p\n", base);

    return 0;
}

bool LocalApic::isPrimaryCpu() {
    uint64_t msr;
    RD_APIC_MSR(msr);

    return (msr & LAPIC_BSP);
}

