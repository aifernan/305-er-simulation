#ifndef PATIENTNODE_H
#define PATIENTNODE_H

class PatientNode {
public:
    int priority;           // (1: low, 2: mid, 3: high)
    double arrival_time;
    double service_time1;
    double service_time2;
    PatientNode* next;

    PatientNode();
    ~PatientNode();
};

#endif