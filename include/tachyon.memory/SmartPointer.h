/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.memory/ReferenceCounter.h>
#include <tachyon.memory/CoreHeap.h>

template<class T>
class SmartPointer {
    ReferenceCounter* cnt;
    T* ptr;
public:
    SmartPointer()
        :   cnt(new ReferenceCounter())
        ,   ptr(0) {
        cnt->up();
    }

    SmartPointer(T* obj)
        :   cnt(new ReferenceCounter())
        ,   ptr(obj) {
        cnt->up();
    }

    SmartPointer(const SmartPointer<T>& rhs)
        :   cnt(rhs.cnt)
        ,   ptr(rhs.ptr) {
        cnt->up();
    }

    ~SmartPointer() {
        if(cnt->down() == 0) {
            delete cnt;
            delete ptr;
        }
    }

    T& operator*() {
        return *ptr;
    }

    T* operator->() {
        return ptr;
    }

    T* get() {
        return ptr;
    }

    SmartPointer<T>& operator=(const SmartPointer<T>& rhs) {
        /* prevent self-asignment */
        if(this != &rhs) {
            if(cnt->down() == 0) {
                delete cnt;
                delete ptr;
            }

            ptr = rhs.ptr;
            cnt = rhs.cnt;
            cnt->up();
        }

        return *this;
    }

    bool operator==(const SmartPointer<T>& rhs) const {
        return (ptr == rhs.ptr);
    }

    size_t references() {
        return cnt->get();
    }
};
