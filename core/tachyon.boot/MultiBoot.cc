/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.boot/MultiBoot.h>

MultiBootInformation::MultiBootInformation(void* mbi)
        :   info(reinterpret_cast<mb_info*>(mbi)),
            mmap_count(0) {

    if(info->flags & MB_INFO_MEMMAP) {
        mmap_count = MemoryRange::countEntries(info->mmap_addr, info->mmap_length);
    }
}

MultiBootInformation::MemoryRange::MemoryRange(uint32_t mmap_base, uint32_t mmap_num)
{
    pinfo = reinterpret_cast<mb_mmap*>(mmap_base);

    while(mmap_num--) {
        pinfo = reinterpret_cast<mb_mmap*>(
            reinterpret_cast<uintptr_t>(pinfo) + pinfo->size + sizeof(pinfo->size)
        );
    }
}

uintmax_t MultiBootInformation::MemoryRange::countEntries(uint32_t mmap_base, uint32_t mmap_length) {
    uintmax_t cnt = 0;

    for(mb_mmap* p = reinterpret_cast<mb_mmap*>(mmap_base);
        reinterpret_cast<uintptr_t>(p) < mmap_base + mmap_length;
        p = reinterpret_cast<mb_mmap*>(
            reinterpret_cast<uintptr_t>(p) + p->size + sizeof(p->size))
    ) {
        ++cnt;
    }

    return cnt;
}

MultiBootInformation::MemoryRange MultiBootInformation::getMemRange(uintmax_t index) {
    return MemoryRange(info->mmap_addr, index);
}
