#ifndef ROOM_H
#define ROOM_H

class Room {
public:
    int status;         // 0 = Available ; 1 = Occupied ; 2 = Requires Cleaning; 3 = Being Cleaned currently
    double arrivalTime;    // For cleaning
    double serviceTime;    // Time it takes to clean, or is this suppose to be for 
    Room* next;

    Room(int s, double at, double st);
    ~Room();
};

// I was thinking of doing 2 lists. One for Availability & another for cleaningR
class RoomList{
public:
    int length;
    Room* head;
    Room* tail;

    RoomList();
    ~RoomList();
    void insert(Room* r);
    Room* removeHeadRoom();
    Room* removeRoom(int s);
    Room* removeRoom(Room* r);
    int getLength();
};

#endif