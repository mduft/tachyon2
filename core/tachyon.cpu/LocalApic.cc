/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.logging/Log.h>
#include <tachyon.cpu/LocalApic.h>
#include <tachyon.platform/x86/InterruptManager.h>
#include <tachyon.logging/Log.h>

#include <tachyon.memory/PhysicalMemory.h>
#include <tachyon.memory/VirtualMemory.h>
#include <tachyon.memory/VirtualZoneManager.h>

#if defined(__X86__)
# define RD_APIC_MSR(x) asm volatile("rdmsr" : "=A"(x) : "c"(IA32_APIC_BASE));
#elif defined (__X86_64__)
# define RD_APIC_MSR(x) asm volatile("xor %%rax, %%rax; rdmsr; shl $31, %%rbx; or %%rbx, %%rax;" : "=a"(x) : "c"(IA32_APIC_BASE) : "%rbx");
#endif

#define WR_APIC_MSR(l) asm volatile("wrmsr" :: "d"(0), "a"(l), "c"(IA32_APIC_BASE));

#define APIC_REG(x)   *(reinterpret_cast<uint32_t*>(APIC_VIRTUAL + x))

/* 
 * .--------------------------------.
 * | APIC values.                   |
 * '--------------------------------' 
 */

#define APIC_GLOBAL_ENABLE      (1 << 11)
#define APIC_BSP                (1 << 8)

#define APIC_REG_ID             0x20
#define APIC_REG_VERSION        0x30
#define APIC_REG_TPR            0x80
#define APIC_REG_APR            0x90
#define APIC_REG_CPR            0xa0
#define APIC_REG_EOI            0xb0
#define APIC_REG_REMOTE_READ    0xc0
#define APIC_REG_LOGIC_DEST     0xd0
#define APIC_REG_DEST_FORMAT    0xe0
#define APIC_REG_SV             0xf0

#define APIC_REG_ISR0           0x100
#define APIC_REG_ISR1           0x110
#define APIC_REG_ISR2           0x120
#define APIC_REG_ISR3           0x130
#define APIC_REG_ISR4           0x140
#define APIC_REG_ISR5           0x150
#define APIC_REG_ISR6           0x160
#define APIC_REG_ISR7           0x170

#define APIC_REG_TMR0           0x180
#define APIC_REG_TMR1           0x190
#define APIC_REG_TMR2           0x1a0
#define APIC_REG_TMR3           0x1b0
#define APIC_REG_TMR4           0x1c0
#define APIC_REG_TMR5           0x1d0
#define APIC_REG_TMR6           0x1e0
#define APIC_REG_TMR7           0x1f0

#define APIC_REG_IRR0           0x200
#define APIC_REG_IRR1           0x210
#define APIC_REG_IRR2           0x220
#define APIC_REG_IRR3           0x230
#define APIC_REG_IRR4           0x240
#define APIC_REG_IRR5           0x250
#define APIC_REG_IRR6           0x260
#define APIC_REG_IRR7           0x270

#define APIC_REG_ERROR_STATUS   0x280
#define APIC_REG_LVT_CMCI       0x2f0

#define APIC_REG_ICR0           0x300
#define APIC_REG_ICR1           0x310

#define APIC_REG_LVT_TIMER      0x320
#define APIC_REG_LVT_THERMAL    0x330
#define APIC_REG_LVT_PERF       0x340
#define APIC_REG_LVT_LINT0      0x350
#define APIC_REG_LVT_LINT1      0x360
#define APIC_REG_LVT_ERROR      0x370

#define APIC_REG_INITIAL_COUNT  0x380
#define APIC_REG_CURRENT_COUNT  0x390
#define APIC_REG_DIVIDE_CONFIG  0x3e0

#define APIC_SV_ENABLE          (1 << 8)
#define APIC_SV_NO_EOI_BCAST    (1 << 12)

#define APIC_TM_PERIODIC        (1 << 17)
#define APIC_TM_TSC_DEADL       (2 << 17)

#define APIC_TIMER_MASKED       (1 << 16)
#define APIC_TIMER_VECTOR       0x20

#define APIC_LVT_SEND_PENDING   (1 << 12)

#define APIC_DM_FIXED           0x0
#define APIC_DM_SMI             0x2
#define APIC_DM_NMI             0x4
#define APIC_DM_INIT            0x5
#define APIC_DM_EXT             0x7

#define APIC_LVT_TRIGGER_LEVEL  (1 << 15)
#define APIC_LVT_REMOTE_IRR     (1 << 14)
#define APIC_LVT_IN_PIN_POL     (1 << 13)
#define APIC_LVT_MASKED         (1 << 17)

#define IRQ_SPURIOUS        0x20
#define IRQ_TIMER           0x21
#define IRQ_ERROR           0x22

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

    return (msr & APIC_BSP);
}

bool LocalApic::isEnabled() {
    uint64_t msr;
    RD_APIC_MSR(msr);

    return (msr & APIC_GLOBAL_ENABLE && (APIC_REG(APIC_REG_SV) & APIC_SV_ENABLE));
}

static void spuriousHandler(interrupt_t* state) {
    KINFO("spurious interrupt!\n");
    LocalApic::eoi();
}

static void errorHandler(interrupt_t* state) {
    KWARN("APIC error!\n");
    LocalApic::eoi();
}

static void timerHandler(interrupt_t* state) {
    static uint64_t c;
    KINFO("timer: %d\n", ++c);
    LocalApic::eoi();
}

void LocalApic::init() {
    /* setup the local APIC. the APIC timer is used for scheduling,
     * so it needs to be setup the same way for all cpus */
    KINFO("APIC base @ %p\n", getPhysicalBase());

    /* Local APIC MSR page must be reserved, and mapped to a known good address. */
    PhysicalMemory::instance().reserve(getPhysicalBase(), getPhysicalBase() + 0x1000);
    VirtualZone* apicZone = VirtualZoneManager::instance().define(APIC_VIRTUAL, APIC_VIRTUAL + 0x1000);
    apicZone->used(true);
    VirtualMemory::instance().map(VirtualMemory::instance().getCurrentVSpace(), 
        APIC_VIRTUAL, getPhysicalBase(), PAGE_WRITABLE | PAGE_NONCACHABLE | PAGE_WRITETHROUGH);

    /* Sanity check: we really want to be on the BSP here,
     * since we did not initialize SMP yet... */
    if(!isPrimaryCpu()) {
        KFATAL("not on primary CPU!\n");
    }
    
    InterruptManager::instance().addHandler(IRQ_TIMER, timerHandler);
    InterruptManager::instance().addHandler(IRQ_SPURIOUS, spuriousHandler);
    InterruptManager::instance().addHandler(IRQ_ERROR, errorHandler);

    KINFO("APIC version/id: 0x%8x/0x%8x\n", APIC_REG(APIC_REG_VERSION), APIC_REG(APIC_REG_ID));

    /* now program and enable the APIC */
    APIC_REG(APIC_REG_LVT_LINT0)    |= APIC_LVT_MASKED;
    APIC_REG(APIC_REG_LVT_LINT1)    |= APIC_LVT_MASKED;
    APIC_REG(APIC_REG_LVT_THERMAL)  |= APIC_LVT_MASKED;
    APIC_REG(APIC_REG_LVT_PERF)     |= APIC_LVT_MASKED;

    APIC_REG(APIC_REG_SV)           |= IRQ_SPURIOUS | APIC_SV_ENABLE;

    APIC_REG(APIC_REG_DIVIDE_CONFIG) = 0xB;
    APIC_REG(APIC_REG_INITIAL_COUNT) = 0x10000000;
    APIC_REG(APIC_REG_LVT_TIMER)     = APIC_TM_PERIODIC | IRQ_TIMER;

    APIC_REG(APIC_REG_LVT_ERROR)     = IRQ_ERROR;

    /* requires back-to-back write */
    APIC_REG(APIC_REG_ERROR_STATUS)  = 0;
    APIC_REG(APIC_REG_ERROR_STATUS)  = 0;

    APIC_REG(APIC_REG_EOI)           = 0;
    APIC_REG(APIC_REG_TPR)           = 0;
}

void LocalApic::eoi() {
    APIC_REG(APIC_REG_EOI) = 0;
}
