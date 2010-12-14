/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/architecture.h>
#include <tachyon.platform/external.h>
#include <tachyon.platform/TimestampCounter.h>

#include <tachyon.logging/Log.h>
#include <tachyon.boot/MultiBoot.h>
#include <tachyon.memory/MemoryHelper.h>
#include <tachyon.memory/PhysicalAllocator.h>

extern "C" uintptr_t CORE_LMA_START;
extern "C" uintptr_t _core_lma_ebss;

extern "C" void boot(void* mbd, uint32_t mbm) {
    TimestampCounter ctr;

    /* make log output appear on the kernel console.
     * TODO: make this configurable (kernel command line?) */
    Log::instance()->addWriter(__kcon_write);
    Log::instance()->setLevel(Log::Trace);

    KINFO("booting tachyon on %s\n", TACHYON_ARCH);
    KTRACE("multiboot information at %p (magic: 0x%x)\n", mbd, mbm);

    if(mbm != MB_MAGIC) {
        KFATAL("multiboot magic number check failed.\n");
    }

    MultiBootInformation mbi(mbd);

    KINFO("mbi: %dkb lower, and %dmb high memory\n", mbi.getLowMemoryKB(), (mbi.getHighMemoryKB() / 1024));

    if(mbi.getMemRangeCount() > 0) {
        char szStr[3] = "kb";
        KTRACE("mbi: memory map:\n");

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
                PhysicalAllocator::instance().available(range.getStart(), range.getLength());
            }
        }
    }

    /* reserve physical regions special to the kernel. reserved
     * regions from the system are not made available above in
     * the first place, so no need to reserve them now. */
    PhysicalAllocator::instance().reserve(0x0, 0x100000);
    PhysicalAllocator::instance().reserve(reinterpret_cast<uintptr_t>(&CORE_LMA_START),
        (reinterpret_cast<uintptr_t>(&_core_lma_ebss) + 0x1000) & ~0xFFF);

    /* test timestamp counter */
    uint64_t start = ctr.getCurrentTicks();
    uint64_t min = ctr.getCurrentTicks() - start;
    KINFO("min-ticks: %d\n", min);

    /* test allocator */
    uintptr_t phys0 = PhysicalAllocator::instance().allocate();
    uintptr_t phys1 = PhysicalAllocator::instance().allocate(static_cast<uintptr_t>(0x400000));
    uintptr_t phys2 = PhysicalAllocator::instance().allocate(true);
    uintptr_t phys3 = PhysicalAllocator::instance().allocate(0x401000, true);
    uintptr_t phys4 = PhysicalAllocator::instance().allocate();
    uintptr_t phys5 = PhysicalAllocator::instance().allocate(true);

    KINFO("phys-alloc: %p, %p\n", phys0, phys1);
    KINFO("            %p, %p\n", phys2, phys3);
    KINFO("            %p, %p\n", phys4, phys5);

    KFATAL("reached end of kernel after %lu ticks!\n", ctr.getTicksSinceStart());
}
