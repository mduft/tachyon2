/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

class ReferenceCounter {
    size_t count;
public:
    ReferenceCounter()
        :   count(0) {}

    // TODO: locking?
    size_t up()     { return ++count; }
    size_t down()   { return --count; }

    size_t get()    { return count; }
};
