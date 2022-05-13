#include "patientnode.h"
#include "room.h"
#ifndef EVENTNODE_H
#define EVENTNODE_H

class EventNode {
public:
    double time;            // Start time of event
    int stage;              // To specify whether we are in stage 1, 2, or 3 (janitor queue)
    int type;               // 1: arrival, 2: start service, 3: departure, 10: end of simulation
    PatientNode* patient;
    Room* room;
    EventNode* next;

    EventNode(double t, int s, int ty, PatientNode* p, Room* r);
    EventNode(double t, int s, int ty, PatientNode* p);
    EventNode(double t, int s, int ty, Room* r);
    EventNode(double t, int s, int ty);
    ~EventNode();
};

#endif