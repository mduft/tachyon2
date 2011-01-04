/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

template<typename T>
IFace GenericIterator {
    virtual T& operator*() = 0;
    virtual T* operator->() = 0;
    virtual GenericIterator& operator++() = 0;
    virtual bool end() = 0;
};


