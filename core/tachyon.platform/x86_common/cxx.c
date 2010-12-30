/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/architecture.h>

/* 
 * .--------------------------------.
 * | Construction and destruction   |
 * '--------------------------------' 
 */

extern uintptr_t _core_vma_ctors;
extern uintptr_t _core_vma_ectors;
extern uintptr_t _core_vma_dtors;
extern uintptr_t _core_vma_edtors;

void __init_cxx() {
    uintptr_t* ct;

    for(ct = &_core_vma_ctors; ct < &_core_vma_ectors; ++ct) {
        ((void(*)(void))*ct)();
    }
}

void __cleanup_cxx() {
    uintptr_t* dt;

    for(dt = &_core_vma_dtors; dt < &_core_vma_edtors; ++dt) {
        ((void(*)(void))*dt)();
    }
}

/* 
 * .--------------------------------.
 * | Support for the g++ compiler   |
 * '--------------------------------' 
 */

extern void NORETURN abort();

void __cxa_pure_virtual() {
    abort();
}

void* __dso_handle = NULL;

#define MAX_ATEXIT_FUNCS    512

struct atexit_t {
    void (*dt)(void*);
    void* arg;
    void* dso;
};

struct atexit_t __cxa_atexit_funcs[MAX_ATEXIT_FUNCS];
uintptr_t __cxa_atexit_count = 0;

int __cxa_atexit(void(*dt)(void*), void* arg, void* dso) {
    if(__cxa_atexit_count >= MAX_ATEXIT_FUNCS)
        return -1;

    __cxa_atexit_funcs[__cxa_atexit_count].dt  = dt;
    __cxa_atexit_funcs[__cxa_atexit_count].arg = arg;
    __cxa_atexit_funcs[__cxa_atexit_count].dso = dso;
    __cxa_atexit_count++;

    return 0;
}

void __cxa_finalize(void* dt) {
    uintptr_t i = __cxa_atexit_count;
    while(i--) {
        /* if dt == NULL, call all of them, if not, only those matching 
         * (all of those matching!) */
        if(__cxa_atexit_funcs[i].dt && (dt == NULL || dt == __cxa_atexit_funcs[i].dt)) {
            (*__cxa_atexit_funcs[i].dt)(__cxa_atexit_funcs[i].arg);
        }
    }
}

/* 
 * .--------------------------------.
 * | Support for local statics      |
 * '--------------------------------' 
 */

/* The ABI requires a 64-bit type.  */
__extension__ typedef int __guard __attribute__((mode(__DI__)));

int __cxa_guard_acquire (__guard *g) {
    // TODO: lock this!
    return !*(char *)(g);
}

void __cxa_guard_release (__guard *g) {
    // TODO: lock this!
    *(char *)g = 1;
}

void __cxa_guard_abort (__guard *g) {
    (void)g; // UNUSED
}
