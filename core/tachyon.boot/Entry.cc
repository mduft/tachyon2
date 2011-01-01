/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/architecture.h>
#include <tachyon.platform/external.h>
#include <tachyon.platform/TimestampCounter.h>

#include <tachyon.logging/Log.h>
#include <tachyon.boot/MultiBoot.h>
#include <tachyon.memory/MemoryHelper.h>
#include <tachyon.memory/PhysicalMemory.h>
#include <tachyon.memory/VirtualMemory.h>
#include <tachyon.memory/VirtualZoneManager.h>
#include <tachyon.memory/CoreHeap.h>
#include <tachyon.memory/SmartPointer.h>

#include <tachyon.cpu/Cpu.h>
#include <tachyon.cpu/CpuManager.h>
#include <tachyon.cpu/LocalApic.h>

extern "C" uintptr_t CORE_LMA_START;
extern "C" uintptr_t _core_lma_ebss;

extern "C" void boot(void* mbd, uint32_t mbm) {
    TimestampCounter ctr;

    /* make log output appear on the kernel console.
     * TODO: make this configurable (kernel command line?) */
    Log::instance().addWriter(__kcon_write);
    Log::instance().setLevel(Log::Trace);

    KINFO("booting tachyon on %s\n", TACHYON_ARCH);

    /* basic check .. */
    if(reinterpret_cast<uintptr_t>(&_core_lma_ebss) > CORE_HEAP_START)
        KFATAL("bss crosses kernel heap!\n");

    if(mbm != MB_MAGIC) {
        KFATAL("boot magic number check failed.\n");
    }

    MultiBootInformation mbi(mbd);

    KINFO("%dkb lower, and %dmb high memory\n", mbi.getLowMemoryKB(), (mbi.getHighMemoryKB() / 1024));

    if(mbi.getMemRangeCount() > 0) {
        char szStr[3] = "kb";
        KTRACE("memory map:\n");

        for(uintmax_t i = 0; i < mbi.getMemRangeCount(); ++i) {
            MultiBootInformation::MemoryRange range =
                mbi.getMemRange(i);

            szStr[0] = 'k';
            uintmax_t sz = (range.getLength() / 1024);

            if(sz > 1024) {
                sz /= 1024;
                szStr[0] = 'm';
            }

            KTRACE("    0x%16lx - 0x%16lx (%5d%s, %s)\n",
                range.getStart(), range.getEnd(), sz, szStr,
                (range.isAvailable() ? " avail" : "!avail"));

            if(range.isAvailable()) {
                PhysicalMemory::instance().available(range.getStart(), range.getEnd());
            }
        }
    }

    /* reserve physical regions special to the kernel. reserved
     * regions from the system are not made available above in
     * the first place, so no need to reserve them now. */
    PhysicalMemory::instance().reserve(0x0, 0x100000);
    PhysicalMemory::instance().reserve(reinterpret_cast<uintptr_t>(&CORE_LMA_START),
        (reinterpret_cast<uintptr_t>(&CORE_LMA_START) + 
            ((reinterpret_cast<uintptr_t>(&_core_lma_ebss) + 0x1000) & ~0xFFF)));

    /* Local APIC MSR page must be reserved, and mapped to a known good address. */
    PhysicalMemory::instance().reserve(LAPIC_PHYSICAL, LAPIC_PHYSICAL + 0x1000);
    VirtualZone* apicZone = VirtualZoneManager::instance().define(LAPIC_VIRTUAL, LAPIC_VIRTUAL + 0x1000);
    apicZone->used(true);
    VirtualMemory::instance().map(VirtualMemory::instance().getCurrentVSpace(), 
        LAPIC_VIRTUAL, LAPIC_PHYSICAL, PAGE_WRITABLE | PAGE_NONCACHABLE | PAGE_WRITETHROUGH);

    /* Sanity check: we really want to be on the BSP here,
     * since we did not initialize SMP yet... */
    if(!LocalApic::isPrimaryCpu()) {
        KFATAL("not on primary CPU!\n");
    }

    /* Initialize BSP */
    SmartPointer<Cpu> bspCpu = SmartPointer<Cpu>(new Cpu(LocalApic::getId()));
    CpuManager::instance().add(bspCpu);

    /* temporary to see more screen output! */
    asm("cli; hlt;");

    KFATAL("reached end of kernel after %lu ticks!\n", ctr.getTicksSinceStart());
}
