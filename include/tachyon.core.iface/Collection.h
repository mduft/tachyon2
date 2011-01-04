/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.collections/BaseIterator.h>

template<typename T>
IFace Collection {
    typedef BaseIterator<T> Iterator;

    virtual size_t size() = 0;
    virtual void clear() = 0;

    virtual void add(T item) = 0;
    virtual void remove(T item) = 0;
    virtual Iterator iterator() = 0;
};
