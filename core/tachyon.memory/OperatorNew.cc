/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.memory/CoreHeap.h>

#ifdef __X86_64__
typedef long unsigned int opsize_t;
#elif defined(__X86__)
typedef unsigned int opsize_t;
#endif

void *operator new(opsize_t size) {
    return CoreHeap::instance().allocate(size);
}
 
void *operator new[](opsize_t size) {
    return CoreHeap::instance().allocate(size);
}
 
void operator delete(void *p) {
    CoreHeap::instance().free(p);
}
 
void operator delete[](void *p) {
    CoreHeap::instance().free(p);
}

