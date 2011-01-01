/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.core.iface/Collection.h>

template<class T>
class LinkedList : Collection<T> {
public:
    virtual size_t size();
    virtual void clear();

    void add(T);
};
