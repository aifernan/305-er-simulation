#include "eventnode.h"
#ifndef EVENTLIST_H
#define EVENTLIST_H

class EventList {
public:
    EventNode* head;
    EventNode* tail;

    EventList();
    ~EventList();
    void insert(EventNode* e);
    EventNode* removeHead();
};

#endif