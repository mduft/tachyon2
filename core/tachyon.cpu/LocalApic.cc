/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.cpu/LocalApic.h>

#if defined(__X86__)
# define RD_APIC_MSR(x) asm volatile("rdmsr" : "=A"(x) : "c"(IA32_APIC_BASE));
#elif defined (__X86_64__)
# define RD_APIC_MSR(x) asm volatile("xor %%rax, %%rax; rdmsr; shl $31, %%rbx; or %%rbx, %%rax;" : "=a"(x) : "c"(IA32_APIC_BASE) : "%rbx");
#endif

uint32_t* LocalApic::getBase() {
    uintptr_t base;

    RD_APIC_MSR(base);

    return reinterpret_cast<uint32_t*>(base /* TODO: and away flags! */);
}

