#include "patientnode.h"
#include "room.h"
#ifndef QUEUE_H
#define QUEUE_H

// Queue is used for either Patients or Rooms, depending on the stage, BUT never both.
class Queue {
public:
    PatientNode* head;
    PatientNode* tail;
    Room* room_head;         // Might not need
    Room* room_tail;

    int waiting_count;
    double cumulative_waiting;

    Queue();
    ~Queue();
    void stage1_push(PatientNode* p);
    void stage2_push(PatientNode* p);
    void stage3_push(Room* r);
    PatientNode* pop();
    Room* room_pop();
    void initialize(int priority, float lambda, float mu, double sim_duration);
};

#endif