/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.logging/Log.h>
#include <tachyon.cpu/LocalApic.h>
#include <tachyon.platform/x86/InterruptManager.h>
#include <tachyon.logging/Log.h>

#if defined(__X86__)
# define RD_APIC_MSR(x) asm volatile("rdmsr" : "=A"(x) : "c"(IA32_APIC_BASE));
#elif defined (__X86_64__)
# define RD_APIC_MSR(x) asm volatile("xor %%rax, %%rax; rdmsr; shl $31, %%rbx; or %%rbx, %%rax;" : "=a"(x) : "c"(IA32_APIC_BASE) : "%rbx");
#endif

#define LAPICR(x)   (reinterpret_cast<uint32_t*>(LAPIC_VIRTUAL + x))

uintptr_t LocalApic::getPhysicalBase() {
    uint64_t msr;
    RD_APIC_MSR(msr);

    return static_cast<uintptr_t>(msr & 0xFFFFF000);
}

uint32_t LocalApic::getId() {
    return 0;
}

bool LocalApic::isPrimaryCpu() {
    uint64_t msr;
    RD_APIC_MSR(msr);

    return (msr & LAPIC_BSP);
}

bool LocalApic::isEnabled() {
    uint32_t msr;
    RD_APIC_MSR(msr);

    return (msr & LAPIC_GLOBAL_ENABLE && (*LAPICR(LAPIC_R_SPIV) & LAPIC_SPIV_ENABLE));
}

static void spuriousHandler(interrupt_t* state) {
    KINFO("spurious interrupt!\n");
}

static void timerHandler(interrupt_t* state) {
    KINFO("timer: %d\r", *LAPICR(LAPIC_R_CURRENT_COUNT));
}

void LocalApic::init() {
    /* setup the local APIC. the APIC timer is used for scheduling,
     * so it needs to be setup the same way for all cpus */
    
    InterruptManager::instance().addHandler(0x20, timerHandler);
    InterruptManager::instance().addHandler(0x21, spuriousHandler);

    /* inhibit all other interrupts from the APIC for now. */
    *LAPICR(LAPIC_R_LVT_LINT0) |= LAPIC_LVT_MASKED;
    *LAPICR(LAPIC_R_LVT_LINT1) |= LAPIC_LVT_MASKED;
    *LAPICR(LAPIC_R_LVT_THERMAL) |= LAPIC_LVT_MASKED;
    *LAPICR(LAPIC_R_LVT_PERF) |= LAPIC_LVT_MASKED;

    *LAPICR(LAPIC_R_SPIV) = 0x21 | LAPIC_SPIV_ENABLE;

    asm("sti");
}

void LocalApic::eoi() {
    *LAPICR(LAPIC_R_EOI) = 0;
}
