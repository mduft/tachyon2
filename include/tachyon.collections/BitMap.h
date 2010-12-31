/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.core.iface/Collection.h>
#include <tachyon.memory/MemoryHelper.h>

class BitMap : Collection<bool> {
    size_t byteCount;
    uint8_t* memory;
public:
    BitMap(size_t size)
        :   byteCount(size)
        ,   memory(new uint8_t[size]) {}
    BitMap(void* mem, size_t size)
        :   byteCount(size)
        ,   memory(reinterpret_cast<uint8_t*>(mem)) {}
    
    /* Collection<bool> members */
    virtual size_t size() { return (byteCount * 8); }
    virtual bool get(size_t index);
    virtual void set(size_t index, bool value);
    virtual void clear() { MemoryHelper::zero(memory, byteCount); }

    void setAll(bool value) { 
        if(value)   { MemoryHelper::fill(memory, 0xFF, byteCount); }
        else        { MemoryHelper::zero(memory, byteCount); }
    }

    /* Efficient searching */
    ssize_t getIndexOf(bool value);
    ssize_t getIndexOf(bool value, ssize_t start);
};
