#include "eventlist.h"
#include <iostream>
#include <assert.h>

using namespace std;

EventList::EventList() {
    head = NULL;
    tail = NULL;
}

EventList::~EventList() {
    while (head != NULL) {
        delete removeHead();
    }
}

void EventList::insert(EventNode* e) {
    assert(e != NULL);

    if (head == NULL && tail == NULL) {
        // Empty / Initial case
        head = e;
        tail = e;
    
        return;
    } 

    if (head->time > e->time) {
        // Wierd 1 element corner case
        e->next = head;
        tail = head;
        head = e;

        return;
    }
    // Start from head
    EventNode* iter = head;

    // Go to the right spot in time
    while (iter != NULL) {
        if (iter == tail) {
            // We are at the end, insert at the tail
            iter->next = e;
            tail = e;
            return;

        } else if (iter->next->time > e->time) {
            // General case, insert at the right spot
            e->next = iter->next;
            iter->next = e;
            return;
        
        } else if (iter->next->time == e->time) {
        // Corner case where times are the same
        // Priorities: Departure (3) > Arrival (1) > Service Start (2)
        // Service starts (type 2 events) go to the back of the equal-time events
            if (e->type == 1) {
                if (iter->next->type == 2) {
                    // Only insert here if the next event is a service start
                    e->next = iter->next;
                    iter->next = e;
                    return;
                }
            } else if (e->type == 3) {
                e->next = iter->next;
                iter->next = e;
                return;
            }
        }

        iter = iter->next;
    }
}

EventNode* EventList::removeHead() {
    assert(head != NULL);

    EventNode* toDelete = head;
    if (head == tail) {
        head = NULL;
        tail = NULL;
    } else {
        head = head->next;
    }

    return toDelete;
}