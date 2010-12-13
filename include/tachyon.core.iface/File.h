/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

typedef enum {
    Seek_Set,
    Seek_Current,
    Seek_End
} seek_rel_t;

IFace File {
    virtual intmax_t read(void* buffer, size_t numBytes) = 0;
    virtual intmax_t write(void* buffer, size_t numBytes) = 0;
    virtual off_t seek(off_t offset, seek_rel_t relation) = 0;
    virtual off_t tell() = 0;
    virtual bool eof() = 0;
};
