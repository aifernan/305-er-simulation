#include "queue.h"
#include <iostream>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

using namespace std;

Queue::Queue() {
    head = NULL;
    tail = NULL;
    room_head = NULL;
    room_tail = NULL;
    waiting_count = 0;
    cumulative_waiting = 0;
}

Queue::~Queue() {
    PatientNode* toDelete;

    while (head != NULL) {
        toDelete = head;

        if (head == tail) {
            head = NULL;
            tail = NULL;
        } else {
            head = head->next;
        }

        delete toDelete;
    }
    
    waiting_count = 0;
    cumulative_waiting = 0;
}

void Queue::stage1_push(PatientNode* p) {
    assert(p != NULL);

    // Insert at the tail
    if (head == NULL && tail == NULL) {
        head = p;
        tail = p;
    } else {
        tail->next = p;
        tail = p;
    }

    waiting_count++;

    return;
}

void Queue::stage2_push(PatientNode* p) {
    assert(p != NULL);

    // Insert based on priority, then by arrival time

    // If queue is empty
    if (head == NULL && tail == NULL){
        head = p;
        tail = p;
    }

    // If p node is new head (If p has higher priority, or same priority & has smaller arrival time than head)
    else if ( (p->priority > head->priority) || ((p->priority == head->priority) && (head->arrival_time > p->arrival_time)) ){
        p->next = head;
        head = p;
    }

    else {
        PatientNode* currentNode = head;
        PatientNode* previousNode = head;
        // Go through each node to see whats the corresponding node before p node
        while(currentNode != NULL){
            if ((p->priority < currentNode->priority) || (p->arrival_time > currentNode->arrival_time)){
                previousNode = currentNode;
                currentNode = currentNode->next;
            } else 
                break;
        }

        previousNode->next = p;
        p->next = currentNode;

        if (currentNode == NULL){   // If currentNode is NULL (and then previousNode was tail), then p has to be new tail then
            tail = p;
        }
    }

    waiting_count++;

    return;  
}

void Queue::stage3_push(Room* r) {
    assert(r != NULL);

    // Insert at the tail
    if (room_head == NULL && room_tail == NULL) {
        room_head = r;
        room_tail = r;
    }
    else {
        room_tail->next = r;
        room_tail = r;
    }

    waiting_count++;

    return;
}

PatientNode* Queue::pop() {
    assert(waiting_count > 0);

    PatientNode* toDelete = head;

    if (head == tail) {
        head = NULL;
        tail = NULL;
    } else {
        head = head->next;
        toDelete->next = NULL;
    }
    waiting_count--;

    return toDelete;
}
Room* Queue::room_pop() {
    assert(waiting_count > 0);

    Room* toDelete = room_head;

    if (room_head == room_tail) {
        room_head = NULL;
        room_tail = NULL;
    }
    else {
        room_head = room_head->next;
        toDelete->next = NULL;
    }
    waiting_count--;

    return toDelete;
}
