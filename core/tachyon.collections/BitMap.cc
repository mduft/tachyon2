/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.collections/BitMap.h>

#define SET_BIT(num, val) \
    { if(val) { memory[num/sizeof(memory[0])] |= (1 << (num%sizeof(memory[0]))); } \
      else    { memory[num/sizeof(memory[0])] &= ~(1<< (num%sizeof(memory[0]))); } }

#define GET_BIT(num, out) \
    { out = memory[num/sizeof(memory[0])] & (1 << (num%sizeof(memory[0]))); }

bool BitMap::get(size_t index) {
    bool bit;
    GET_BIT(index, bit);
    return bit;
}

void BitMap::set(size_t index, bool value) {
    SET_BIT(index, value);
}

ssize_t BitMap::getIndexOf(bool value) {
    return getIndexOf(value, 0);
}

ssize_t BitMap::getIndexOf(bool value, ssize_t start) {
    if(start < 0)
        start = 0;

    // dumb implementation for now...
    while(start <= (byteCount * sizeof(memory[0]))) {
        if(get(start) == value)
            return start;

        ++start;
    }

    return -1;
}
