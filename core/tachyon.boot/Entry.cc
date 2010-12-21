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

extern "C" uintptr_t CORE_LMA_START;
extern "C" uintptr_t _core_lma_ebss;

extern "C" void boot(void* mbd, uint32_t mbm) {
    TimestampCounter ctr;

    /* make log output appear on the kernel console.
     * TODO: make this configurable (kernel command line?) */
    Log::instance()->addWriter(__kcon_write);
    Log::instance()->setLevel(Log::Trace);

    KINFO("booting tachyon on %s\n", TACHYON_ARCH);
    KTRACE("boot information at %p (magic: 0x%x)\n", mbd, mbm);

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

    /* test memory */
    vspace_t kernelSpace = VirtualMemory::instance().getCurrentVSpace();
    phys_addr_t phys = PhysicalMemory::instance().allocateAligned(0x1000, 0x10000);
    for(int i = 0; i < 0x10; i++) {
        uintptr_t virt = 0x1000000 + (0x1000 * i);
        if(!VirtualMemory::instance().map(kernelSpace, virt, phys + (0x1000 * i), PAGE_USER | PAGE_WRITABLE)) {
            KFATAL("failed to map %p -> %p\n", virt, phys);
        }

        MemoryHelper::fill(reinterpret_cast<void*>(virt), i, 0x1000);

        KINFO("small: %p -> %p\n", virt, phys + (0x1000 * i));
    }

    #ifdef __X86_64__
    phys_addr_t phys2 = PhysicalMemory::instance().allocateAligned(0x200000, 0x200000);
    uintptr_t virt = 0xF2000000;
    if(!VirtualMemory::instance().map(kernelSpace, virt, phys2, PAGE_LARGE | PAGE_USER | PAGE_WRITABLE)) {
        KFATAL("failed to map %p -> %p\n",  virt, phys2);
    }

    MemoryHelper::fill(reinterpret_cast<void*>(virt), 0xAA, 0x200000);

    KINFO("large: %p -> %p\n", virt, phys2);
    #endif


    /* temporary to see more screen output! */
    asm("cli; hlt;");

    KFATAL("reached end of kernel after %lu ticks!\n", ctr.getTicksSinceStart());
}
