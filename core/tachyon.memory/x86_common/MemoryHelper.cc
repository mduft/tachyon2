/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/MemoryHelper.h>

/* TODO: This basic implementations could probably need some optimization... */
#define MEMCPY_B(s, d, c, dir) \
    asm(dir "; rep movsb;" :: "S"(s), "D"(d), "c"(c));

void MemoryHelper::copy(void* source, void* dest, size_t numBytes) {
    if(numBytes == 0)
        return;

    MEMCPY_B(source, dest, numBytes, "cld");
}

void MemoryHelper::move(void* source, void* dest, size_t numBytes) {
    if(numBytes == 0)
        return;

    if(((source < dest) && reinterpret_cast<uint8_t*>(source) + numBytes -1 > dest)
        || ((dest < source) && reinterpret_cast<uint8_t*>(dest) + numBytes -1 > source)) {
        MEMCPY_B(source, dest, numBytes, "cld");
    } else {
        /* copy backwards */
        uint8_t* s = reinterpret_cast<uint8_t*>(source) + numBytes -1;
        uint8_t* d = reinterpret_cast<uint8_t*>(dest) + numBytes -1;

        MEMCPY_B(s, d, numBytes, "std");
    }
}

void MemoryHelper::zero(void* dest, size_t numBytes) {
    fill(dest, 0, numBytes);
}

void MemoryHelper::fill(void* dest, uint8_t byte, size_t numBytes) {
    asm("cld; rep stosb"
        : /* no output */
        : "a" (byte)
        , "c" (numBytes)
        , "D" (dest));
}

void const* MemoryHelper::find(register void const* mem, uint8_t byte, register size_t length) {
    register uint32_t i = 0;
    while(i++ < length) {
        if(*(reinterpret_cast<uint8_t const*>(mem) + i) == byte)
            return (reinterpret_cast<uint8_t const*>(mem) + i);
    }

    return 0;
}

void const* MemoryHelper::rfind(register void const* mem, uint8_t byte, register size_t length) {
    while(length--) {
        if(*(reinterpret_cast<uint8_t const*>(mem) + length) == byte)
            return (reinterpret_cast<uint8_t const*>(mem) + length);
    }

    return 0;
}
