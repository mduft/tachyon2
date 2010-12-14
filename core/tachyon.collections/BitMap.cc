/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.collections/BitMap.h>

bool BitMap::get(size_t index) {
    return false;
}

void BitMap::set(size_t index, bool value) {

}

ssize_t BitMap::getIndexOf(bool value) {
    return getIndexOf(value, 0);
}

ssize_t BitMap::getIndexOf(bool value, ssize_t start) {
    if(start < 0)
        start = 0;

    // dump implementation for now...
    while(start <= (byteCount * 8)) {
        if(get(start) == value)
            return start;

        ++start;
    }

    return -1;
}
