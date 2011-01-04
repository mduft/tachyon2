/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.memory/SmartPointer.h>
#include <tachyon.core.iface/GenericIterator.h>

template<typename T>
class BaseIterator : public GenericIterator<T> {
    typedef SmartPointer<GenericIterator<T> > NextIterator;
    NextIterator next;
public:
    BaseIterator(NextIterator n) : next(n) {}

    virtual T& operator*() { return *(*next); }
    virtual T* operator->() { return (*next).operator->(); }
    virtual GenericIterator<T>& operator++() { return ++(*next); }
    virtual bool end() { return next->end(); }
};

