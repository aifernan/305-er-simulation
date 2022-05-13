#include "eventnode.h"
#include <iostream>
#include <assert.h>

using namespace std;

EventNode::EventNode(double t, int s, int ty) {
    time = t;
    stage = s;
    type = ty;
    patient = NULL;
    room = NULL;
    next = NULL;
}

EventNode::EventNode(double t, int s, int ty, PatientNode* p) {
    assert(p != NULL);
    time = t;
    stage = s;
    type = ty;
    patient = p;
    room = NULL;
    next = NULL;
}

EventNode::EventNode(double t, int s, int ty, Room* r) {
    assert(r != NULL);
    time = t;
    stage = s;
    type = ty;
    patient = NULL;
    room = r;
    next = NULL;
}

EventNode::EventNode(double t, int s, int ty, PatientNode* p, Room* r) {
    assert(p != NULL);
    assert(r != NULL);
    time = t;
    stage = s;
    type = ty;
    patient = p;
    room = r;
    next = NULL;
}

EventNode::~EventNode() {
    time = 0;
    stage = 0;
    type = 0;
    patient = NULL;
    next = NULL;
}