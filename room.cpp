#include "room.h"
#include <iostream>
#include <assert.h>

using namespace std;

//----- Room Commands ----//
Room::Room(int s, double at, double st)
{
    status = s;
    arrivalTime = at;
    serviceTime = st;
    next = NULL;
}

Room::~Room()
{
    status = 0;
    arrivalTime = 0;
    serviceTime = 0;
    next = NULL;
}

//-- Room List Commands --//
RoomList::RoomList()
{
    head = NULL;
    tail = NULL;
    length = 0;
}

RoomList::~RoomList()
{
    while (head != NULL)
        delete removeHeadRoom();
}

void RoomList::insert(Room *r)
{
    // cout << "Inserting Room... " << r << " " << r->status << endl;
    assert(r != NULL);
    // Insert at the tail
    if (head == NULL && tail == NULL) {
        head = r;
        tail = r;
    } else {
        tail->next = r;
        tail = r;
    }
    tail->next = NULL;
    length++;

    return;
}

Room *RoomList::removeHeadRoom()
{
    Room *roomToDelete = head;

    if (head == tail)
    {
        head = NULL;
        tail = NULL;
    }
    else {
        head = head->next;
        roomToDelete->next = NULL;
    }

    length--;
    return roomToDelete;
}

Room *RoomList::removeRoom(int s)
{
    Room *curr = head;
    Room *prev = head;

    if (head == tail) {
        head = NULL;
        tail = NULL;
        return curr;
    }

    if (head->status == s){
        return removeHeadRoom();
    } else {
        while (curr != NULL && curr->status != s){
            prev = curr;
            curr = curr->next;
        }

        prev->next = curr->next;
    }

    curr->next = NULL;
    length--;
    return curr;
}

Room *RoomList::removeRoom(Room *r)
{
    Room *curr = head;
    Room *prev = head;

    if (head == tail) {
        head = NULL;
        tail = NULL;
        return curr;
    }

    if (head == r){
        curr = removeHeadRoom();
    } else {
        while (curr != tail && curr != r){
            prev = curr;
            curr = curr->next;
        }

        if (curr == tail) {
            prev->next = NULL;
            tail = prev;
            return curr;
        } 

        prev->next = curr->next;
        curr->next = NULL;
    }

    length--;
    return curr;
}

int RoomList::getLength(){
    return length;
}