/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.core.iface/Collection.h>

template<class T>
class LinkedList : public Collection<T> {
    size_t count;

    typedef struct __tag_llnode_t {
        struct __tag_llnode_t* next;
        T payload;
    } llnode_t;

    llnode_t* head;
public:
    class Iterator {
        friend class LinkedList;

        llnode_t* current;
        Iterator(llnode_t* start) { current = start; }
    public:
        T operator*() { if(!current) { return 0; } return current->payload; }
        void operator++() { if(!current) { return; } current = current->next; }
        bool end() { return (current == 0); }
    };

    LinkedList()
        :   count(0)
        ,   head(0) {}

    virtual size_t size() { return count; }
    virtual void clear() { while(head) { remove(head->payload); } }

    virtual void add(T item) {
        llnode_t* node = new llnode_t;

        node->payload = item;
        node->next = head;
        head = node;

        ++count;
    }

    virtual void remove(T item) {
        llnode_t* prev = 0;
        for(llnode_t* p = head; p; p = p->next) {
            if(p->payload == item) {
                if(prev) {
                    prev->next = p->next;
                } else {
                    head = p->next;
                }

                delete p;
                --count;
            }

            prev = p;
        }
    }

    virtual Iterator iterator() {
        return Iterator(head);
    }
};
