/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.collections/BitMap.h>
#include <tachyon.logging/Log.h>

#define ITEM_BITS (sizeof(memory[0])*8)

#define SET_BIT(num, val) \
    { if(val) { memory[num/ITEM_BITS] |= (1 << (num%ITEM_BITS)); } \
      else    { memory[num/ITEM_BITS] &= ~(1<< (num%ITEM_BITS)); } }

#define GET_BIT(num, out) \
    { out = memory[num/ITEM_BITS] & (1 << (num%ITEM_BITS)); }

#define CHECK_INDEX(i) \
    if((i / ITEM_BITS) >= byteCount) { \
        KFATAL("index out of range: %d (item-size: %d bits)!\n", i, ITEM_BITS); }

bool BitMap::get(size_t index) {
    bool bit;

    CHECK_INDEX(index);

    GET_BIT(index, bit);
    return bit;
}

void BitMap::set(size_t index, bool value) {
    CHECK_INDEX(index);
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
