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
#include <tachyon.cpu/CpuId.h>

#include <tachyon.processes/Scheduler.h>
#include <tachyon.processes/IdleThread.h>

/* little helper with C++ linkage, so the Process class can
 * have this as friend, and allow creation of a process with
 * an existing address space (initialized during boot(), see
 * below).
 */
Process* createCore() {
    return new Process(VirtualMemory::instance().getCurrentVSpace(), 0);
}

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

    /* CPU identification and basic checks */
    idleaf_t cpuid = CpuId::getLeaf(1, 0);
    
    KINFO("cpu identification: family %d, model %d, stepping %d\n", 
        (cpuid.ax >> 8) & 0xF, (cpuid.ax >> 4) & 0xF, (cpuid.ax) & 0xF);
    KINFO("                    extended family %d, extended model %d\n",
        (cpuid.ax >> 20) & 0xFF, (cpuid.ax >> 16) & 0xF);

    if(cpuid.dx & (1 << 9)) {
        LocalApic::init();
        KINFO("APIC state: %s\n", LocalApic::isEnabled() ? "enabled" : "disabled");

        /* Initialize BSP */
        CpuManager::instance().add(SmartPointer<Cpu>(new Cpu(LocalApic::getId())));
    } else {
        KFATAL("APIC support is required!\n");
    }

    /* Initialize process management. Create the kernel process, and
     * add the required threads. */

    SmartPointer<Process> core(createCore());
    
    core->addThread(new Thread(core.get(), &IdleThread::idle, 0));

    asm("sti;");
    asm("hlt;");

    /* should never come here: the STI enables interrupts,
     * the HLT makes the code wait for an interrupt. the only
     * unmasked interrupt here (provided all spurious interrupts
     * have been delivered now), is the schduler timer, which
     * will make the Scheduler switch to another thread. Since
     * there is no Thread for _this_ current location, the
     * scheduler will never return here.
     */

    KFATAL("reached end of kernel after %lu ticks!\n", ctr.getTicksSinceStart());
}
