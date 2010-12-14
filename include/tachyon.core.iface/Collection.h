/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

template<typename T>
IFace Collection {
    virtual size_t size() = 0;
    virtual T get(size_t index) = 0;
    virtual void set(size_t index, T value) = 0;
    virtual void clear() = 0;
};
