#include "patientnode.h"
#include <iostream>

using namespace std;

PatientNode::PatientNode() {
    priority = 0;
    arrival_time = 0;
    service_time1 = 0;
    service_time2 = 0;
    next = NULL;
}

PatientNode::~PatientNode() {
    priority = 0;
    arrival_time = 0;
    service_time1 = 0;
    service_time2 = 0;
    next = NULL;
}