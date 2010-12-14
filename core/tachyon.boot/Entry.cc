/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/architecture.h>
#include <tachyon.platform/external.h>
#include <tachyon.platform/TimestampCounter.h>

#include <tachyon.logging/Log.h>
#include <tachyon.boot/MultiBoot.h>
#include <tachyon.memory/MemoryHelper.h>

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
        }
    }

    uint64_t start = ctr.getCurrentTicks();
    uint64_t min = ctr.getCurrentTicks() - start;
    KINFO("min-ticks: %d\n", min);

    KFATAL("reached end of kernel after %d ticks!\n", ctr.getTicksSinceStart());
}
