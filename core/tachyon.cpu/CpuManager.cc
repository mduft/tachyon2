/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.cpu/CpuManager.h>

CpuManager& CpuManager::instance() {
    static CpuManager inst;
    return inst;
}

