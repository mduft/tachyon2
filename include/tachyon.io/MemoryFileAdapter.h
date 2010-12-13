/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.core.iface/File.h>

class MemoryFileAdapter : public File {
    void* fBase;
    uintmax_t curOff;
    uintmax_t length;
public:
    MemoryFileAdapter(void* base, off_t len) 
        :   fBase(base),
            curOff(0),
            length(len) {}

    /* IFile members */
    virtual intmax_t read(void* buffer, size_t numBytes);
    virtual intmax_t write(void* buffer, size_t numBytes);
    virtual off_t seek(off_t offset, seek_rel_t relation);
    virtual off_t tell() { return curOff; }
    virtual bool eof() { return (curOff == length); }
};
