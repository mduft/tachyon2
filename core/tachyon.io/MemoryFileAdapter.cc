/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.io/MemoryFileAdapter.h>
#include <tachyon.memory/MemoryHelper.h>

intmax_t MemoryFileAdapter::read(void* buffer, size_t numBytes) {
    size_t bytesToCopy = numBytes;

    if(curOff + numBytes > length)
        bytesToCopy = length - curOff;

    MemoryHelper::copy(reinterpret_cast<uint8_t*>(fBase) + curOff, buffer, bytesToCopy);

    curOff += bytesToCopy;
    return bytesToCopy;
}

intmax_t MemoryFileAdapter::write(void* buffer, size_t numBytes) {
    size_t bytesToCopy = numBytes;

    if(curOff + numBytes > length)
        bytesToCopy = length - curOff;

    MemoryHelper::copy(buffer, reinterpret_cast<uint8_t*>(fBase) + curOff, bytesToCopy);

    curOff += bytesToCopy;
    return bytesToCopy;
}

off_t MemoryFileAdapter::seek(off_t offset, seek_rel_t relation) {
    switch(relation) {
    case Seek_Set:
        if(offset < 0 || static_cast<uintmax_t>(offset) > length)
            return -1;

        curOff = offset;
        break;
    case Seek_Current:
        if(static_cast<intmax_t>(curOff) + offset < 0 || curOff + offset > length)
            return -1;

        curOff += offset;
        break;
    case Seek_End:
        if(offset > 0 || (-offset) > length)
            return -1;

        curOff += offset;
        break;
    }
    return curOff;
}
