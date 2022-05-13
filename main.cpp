#include <iostream>
#include <iomanip>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "eventlist.h"
#include "queue.h"
#include "room.h"

using namespace std;

/* GLOBAL VARIABLES */
// Can play with these two
static int simulation_duration = 1440;          // 24 hours = 1440 minutes
static int print_period = 60;                   // Print every hour

static double simulated_stats[11];              // [n, r_all, r_low, r_mid, r_high, w_eq, w_pq_all, w_pq_low, w_pq_med, w_pq_hi, w_cq]
EventNode* current_event = NULL;
double cumulative_number_time_interval_start = 0;
int countOfRoomsBeingUsed = 0;

// testing vars//
int statementTrue = 0;
int totalPatientsArriving = 0;
int totalPatientsGettingToStage2ser = 0;
int totalPatientsGettingToStage2arr = 0;
// Stats for the whole system
int current_number_patients = 0;
int total_high_patients = 0;
int total_med_patients = 0;
int total_low_patients = 0;
int rejected_patients = 0;
int departure_count_stage1 = 0;
int departure_count_stage2 = 0;
int busy_janitors = 0;

double total_arrival_times = 0;
double cumulative_number = 0;
double cumulative_waiting_time_high_priorty = 0;
double cumulative_waiting_time_med_priorty = 0;
double cumulative_waiting_time_low_priorty = 0;
double cumulative_response_all = 0;
double cumulative_response_high = 0;
double cumulative_response_mid = 0;
double cumulative_response_low = 0;
double cumulative_response_cleanUp = 0;
double cumulative_room_calls = 0;

PatientNode *next_arrival = NULL;
PatientNode *next_arrival_low = NULL;
PatientNode *next_arrival_mid = NULL;
PatientNode *next_arrival_high = NULL;

/* HELPER METHODS */
PatientNode *threeMinArrival(PatientNode *a, PatientNode *b, PatientNode *c)
{
    if (a->arrival_time < b->arrival_time)
    {
        if (a->arrival_time < c->arrival_time)
        {
            return a;
        }
        else
        {
            return c;
        }
    }
    else
    {
        if (b->arrival_time < c->arrival_time)
        {
            return b;
        }
        else
        {
            return c;
        }
    }
}

double GenerateRandomNumber()
{
    return ((double)rand() / ((double)RAND_MAX + 1));
}

// Returns the index of the first available server in the server array
int findAvailableServer(PatientNode *servers[], int size)
{
    for (int i = 0; i < size; i++)
    {
        if (servers[i] == NULL)
            return i;
    }

    return -1;
}

// Returns the time of the earliest departure
double firstDeparture(double departure_times[], int size)
{
    double min = departure_times[0];
    for (int i = 1; i < size; i++)
    {
        if (departure_times[i] < min)
            min = departure_times[i];
    }

    return min;
}

Room *findFirstRoom(int status, RoomList *listOfRooms)
{

    Room *currentRoom = listOfRooms->head;
    while (currentRoom != NULL)
    {
        if (status == currentRoom->status)
        {
            return currentRoom;
        }
        currentRoom = currentRoom->next;
    }
    return NULL;
}

void PrintTime(double t_plus) {
    int start_hour = 0;         // We start at 12 am

    int t_plus_hour = (int)(t_plus / 60);
    int t_plus_min = (int)t_plus % 60;

    if (t_plus_hour == 24) t_plus_hour = 0; 

    if (t_plus_min == 0) cout << "Current Time: " << t_plus_hour + start_hour << ":" << t_plus_min << "0" << endl;
    else cout << "Current Time: " << t_plus_hour + start_hour << ":" << t_plus_min << endl;
}

/* SIMULATOR PROCESSES */
PatientNode* CreatePatient(double prev_arrival_time, int priority, float lambda, float mu_1, float mu_2) {
    PatientNode* p = new PatientNode();
    assert(p != NULL);

    double u = GenerateRandomNumber();
    double interArrivalTime;
    if (lambda == 0)
    {
        interArrivalTime = simulation_duration + 1; // This priority group will never show up
    }
    else
    {
        interArrivalTime = -(log(1 - u) / lambda);
    }

    p->arrival_time = prev_arrival_time + interArrivalTime;

    u = GenerateRandomNumber();
    if (mu_1 == 0)
    {
        p->service_time1 = 0;
    }
    else
    {
        p->service_time1 = -(log(1 - u) / mu_1);
    }

    u = GenerateRandomNumber();
    if (mu_2 == 0)
    {
        p->service_time2 = 0;
    }
    else
    {
        p->service_time2 = -(log(1 - u) / mu_2);
    }

    p->priority = priority;
    p->next = NULL;

    return p;
}

EventList *InitializeEventlist()
{
    EventList *l = new EventList();
    assert(l != NULL);

    // Insert a "end of simulation event" to mark the end
    l->insert(new EventNode(simulation_duration, -1, 10));

    return l;
}

void printQueue(Queue *q)
{
    Room *curr = q->room_head;
    int i = 1;
    cout << "Queue: ";
    while (curr != NULL)
    {
        cout << i << ": " << curr->status << " - ";
        curr = curr->next;
        i++;
    }
    cout << endl;
    return;
}

void printList(RoomList *q){
    Room *curr = q->head;
    int i = 1;
    cout << "List: ";
    while (curr != NULL)
    {
        cout << i << ": " << curr->status << " - ";
        curr = curr->next;
        i++;
    }
    cout << endl;
    return;
}

void PrintStatistics(Queue* EQ, Queue* PQ, Queue* CQ) {
    // Calculate and print stats
    // [n, r_all, r_low, r_mid, r_high, w_eq, w_pq_all, w_pq_low, w_pq_med, w_pq_hi, w_cq]
    simulated_stats[0] = cumulative_number / current_event->time;                       // n
    simulated_stats[1] = cumulative_response_all / departure_count_stage2;              // r_all
    simulated_stats[2] = cumulative_response_low / total_low_patients;                  // r_low
    simulated_stats[3] = cumulative_response_mid / total_med_patients;                  // r_med
    simulated_stats[4] = cumulative_response_high / total_high_patients;                // r_high
    simulated_stats[5] = EQ->cumulative_waiting / departure_count_stage1;               // w_eq
    simulated_stats[6] = PQ->cumulative_waiting / departure_count_stage2;               // w_pq_all
    simulated_stats[7] = cumulative_waiting_time_low_priorty / total_low_patients;      // w_pq_low
    simulated_stats[8] = cumulative_waiting_time_med_priorty / total_med_patients;      // w_pq_med
    simulated_stats[9] = cumulative_waiting_time_high_priorty / total_high_patients;    // w_pq_hi
    simulated_stats[10] = cumulative_response_cleanUp / cumulative_room_calls;               // w_cq

    if (current_event->type == 10) cout << "End of Simulation - ";
    PrintTime(current_event->time);
    cout << "Departure Count: " << departure_count_stage2 << endl;
    cout << "Mean n \t\t\t\t\t" << fixed << setprecision(4) << simulated_stats[0] << endl;
    cout << "Mean r\t\t\t\t\t" << fixed << setprecision(4) << simulated_stats[1] << endl;
    cout << "Mean r for low priority\t\t\t" << fixed << setprecision(4) << simulated_stats[2] << endl;
    cout << "Mean r for medium priority\t\t" << fixed << setprecision(4) << simulated_stats[3] << endl;
    cout << "Mean r for high priority\t\t" << fixed << setprecision(4) << simulated_stats[4] << endl;
    cout << "Mean r for C Queue (rooms)\t\t" << fixed << setprecision(4) << simulated_stats[10] << endl;
    cout << "Mean w for E Queue\t\t\t" << fixed << setprecision(4) << simulated_stats[5] << endl;  
    cout << "Mean w for P Queue\t\t\t" << fixed << setprecision(4) << simulated_stats[6] << endl;  
    cout << "Mean w for P Queue (low priority)\t" << fixed << setprecision(4) << simulated_stats[7] << endl;
    cout << "Mean w for P Queue (medium priority)\t" << fixed << setprecision(4) << simulated_stats[8] << endl;
    cout << "Mean w for P Queue (high priority)\t" << fixed << setprecision(4) << simulated_stats[9] << endl;
    cout << "Rejected patients\t\t\t" << rejected_patients << endl << endl;  
}

void Stage1_Arrival(Queue *EQ, EventList *eventL, int capacity, PatientNode *servers[], int num_servers, double departure_times[],
                    float lambda_low, float lambda_mid, float lambda_high, float mu_er, float mu_low, float mu_mid, float mu_high)
{
    totalPatientsArriving++;
    assert(EQ != NULL);
    assert(eventL != NULL);
    assert(current_event->type == 1);

    PatientNode *arrival = current_event->patient;
    bool willDeleteArrival = false;

    if (current_number_patients < capacity)
    {
        // Process arrival
        if (EQ->waiting_count == 0)
        {
            // Empty queue
            // Look for an available server                 // MIGHT WANT TO TURN THIS INTO A HELPER?
            int available_server = findAvailableServer(servers, num_servers);

            if (available_server != -1)
            {
                // There is one
                // Set next service start time to be the current time
                eventL->insert(new EventNode(current_event->time, 1, 2, current_event->patient));

            } else {
                // All servers busy 
                // Set next service start to be the earliest departure
                double fd = firstDeparture(departure_times, num_servers);
                eventL->insert(new EventNode(fd, 1, 2, current_event->patient));
            }
        }

        // For a non-empty queue, there is nothing to do besides pushing them
        EQ->stage1_push(arrival);

        // Update stats
        // Number of patients in the system has changed, update the cumulative number stat
        cumulative_number += current_number_patients * (current_event->time - cumulative_number_time_interval_start);
        cumulative_number_time_interval_start = current_event->time;
        current_number_patients++;
    }
    else
    {
        // If patient got turned away due to capacity issues, update stats, and clean up
        rejected_patients++;
        willDeleteArrival = true;
    }

    // Calculate next up in arrival's priority group
    switch (current_event->patient->priority)
    {
    case 1:
        next_arrival_low = CreatePatient(current_event->time, 1, lambda_low, mu_er, mu_low);
        break;
    case 2:
        next_arrival_mid = CreatePatient(current_event->time, 2, lambda_mid, mu_er, mu_mid);
        break;
    case 3:
        next_arrival_high = CreatePatient(current_event->time, 3, lambda_high, mu_er, mu_high);
        break;
    }

    // Figure out next arrival, create and schedule arrival event for this node
    next_arrival = threeMinArrival(next_arrival_low, next_arrival_mid, next_arrival_high);
    eventL->insert(new EventNode(next_arrival->arrival_time, 1, 1, next_arrival));

    if (willDeleteArrival)
        delete arrival;
}

void Stage1_StartService(Queue *EQ, EventList *eventL, PatientNode *servers[], double departure_times[], int num_servers)
{
    assert(EQ != NULL);
    assert(eventL != NULL);

    // Assign to correct server, in this case take the first available one
    PatientNode *next_patient = EQ->pop();
    assert(next_patient == current_event->patient);

    int available_server = findAvailableServer(servers, num_servers);
    assert(available_server != -1);
    assert(servers[available_server] == NULL);

    servers[available_server] = next_patient;

    // Update departure time of this server
    departure_times[available_server] = current_event->time + next_patient->service_time1;

    // Update queue
    if (EQ->waiting_count > 0)
    {
        // Queue will still be filled, update queue head (done in pop())
        // Schedule next start service to when a server will be free again
        eventL->insert(new EventNode(firstDeparture(departure_times, num_servers), 1, 2, EQ->head));
    }
    // If queue will be empty, update queue pointers (done in pop())

    // Update stats
    // Calculate wait time of this patient
    EQ->cumulative_waiting += current_event->time - next_patient->arrival_time;

    // Schedule departure of this patient
    eventL->insert(new EventNode(departure_times[available_server], 1, 3, next_patient));
}

void Stage1_Departure(Queue *EQ, EventList *eventL, PatientNode *servers[], double departure_times[], int num_servers)
{
    assert(EQ != NULL);
    assert(eventL != NULL);

    // Update stats (none here)

    // Find the server, make it available
    for (int i = 0; i < num_servers; i++)
    {
        if (servers[i] == current_event->patient)
        {
            servers[i] = NULL;
            departure_times[i] = 0;
        }
    }

    departure_count_stage1++;         // Increment total departures from system

    // Schedule a stage 2 arrival for this patient
    eventL->insert(new EventNode(current_event->time, 2, 1, current_event->patient));
}

void Stage2_Arrival(Queue *PQ, EventList *eventL, int totalRooms, RoomList *listOfRooms)
{

    // Tasks:
    // - Schedule start service event
    // - Check if all rooms are being used, if not, try to place them in right away
    // - Add to cumulative num of patients
    totalPatientsGettingToStage2arr++;
    assert(PQ != NULL); // Checks for NULL/incorrect parameter values
    assert(eventL != NULL);
    assert(current_event->type == 1);

    PatientNode *patientInEvent = current_event->patient; // Get Patient & set new Arrival time for Stage 2

    patientInEvent->arrival_time = current_event->time;

    if (countOfRoomsBeingUsed >= totalRooms)
    { // If all rooms are used up, put in Patient Queue only
        // Puts patient in waiting room
        PQ->stage2_push(patientInEvent);
    }
    else
    { // If not, put in Queue & schedule Start Service event
        // Schedule start service event
        PQ->stage2_push(patientInEvent);
        eventL->insert(new EventNode(current_event->time, 2, 2, patientInEvent));
    }

    return;
}

void Stage2_StartService(Queue *PQ, EventList *eventL, RoomList *listOfRooms, Queue* CQ)
{
    // Tasks:
    // - Schedule Departure
    // - Record waiting time of patient
    // - Mark room being occupied/used
    // - Add to cumulative num of patients

    assert(PQ != NULL); // Check for NULL/incorrect parameter values
    assert(eventL != NULL);
    assert(listOfRooms != NULL);
    assert(current_event->type == 2);

    Room *pickingRoom = findFirstRoom(0, listOfRooms); // Finds first room with Status "0" which means its free & clean
    if (pickingRoom == NULL)
    { // If somehow theres no room with status "0", then don't do anything
        //       statementTrue++;
        return;
    }

    pickingRoom->status = 1; // Set room to be "1" = "Occupied"

    PatientNode *patientGoingToRoom = PQ->pop(); // Remove patient from Patient Queue
    assert(patientGoingToRoom != NULL);
    if (patientGoingToRoom == NULL)
    { // If somehow patient is NULL, then don't do anything
        return;
    }

    // Modify Statistic Variables
    PQ->cumulative_waiting += current_event->time - patientGoingToRoom->arrival_time; // Waiting time for Patient Queue
    switch (patientGoingToRoom->priority)
    {
    case 1:
        cumulative_waiting_time_low_priorty += current_event->time - patientGoingToRoom->arrival_time; // Waiting time for Low Prio Patients
        total_low_patients++;
        break;
    case 2:
        cumulative_waiting_time_med_priorty += current_event->time - patientGoingToRoom->arrival_time; // Waiting time for Medium Prio Patients
        total_med_patients++;
        break;
    case 3:
        cumulative_waiting_time_high_priorty += current_event->time - patientGoingToRoom->arrival_time; // Waiting time for High Prio Patients
        total_high_patients++;
        break;
    }

    countOfRoomsBeingUsed++; // Increment number of Rooms being used currently
    totalPatientsGettingToStage2ser++;
    // Insert Departure event into Event List
    eventL->insert(new EventNode((current_event->time + patientGoingToRoom->service_time2), 2, 3, patientGoingToRoom, pickingRoom));
    return;
}

void Stage2_Departure(EventList *eventL, RoomList *listOfRooms, Queue *PQ)
{

    // Tasks:
    // - Schedule cleaning
    // - Set room to be cleaned
    // - Record response time
    // - Set arrival time for room
    // - Add to cumulative number of patients

    assert(eventL != NULL); // Check for NULL/incorrect parameter values
    assert(listOfRooms != NULL);
    assert(current_event->type == 3);

    Room *roomToClear = current_event->room; // Fetches Room and Patient

    PatientNode *patientLeaving = current_event->patient;

    if (roomToClear == NULL || patientLeaving == NULL)
    { // If somehow fetched variables are NULL, then don't do anything
        return;
    }

    // Modify Statistic Variables
    cumulative_response_all += current_event->time - patientLeaving->arrival_time; // Response Time for Patient
    switch (patientLeaving->priority)
    {
    case 1:
        cumulative_response_low += current_event->time - patientLeaving->arrival_time; // Response Time for High Prio Patient
        break;
    case 2:
        cumulative_response_mid += current_event->time - patientLeaving->arrival_time; // Response Time for Medium Prio Patient
        break;
    case 3:
        cumulative_response_high += current_event->time - patientLeaving->arrival_time; // Response Time for Low Prio Patient
        break;
    }

    roomToClear->status = 2;                        // Set Room to be Cleaned
    roomToClear->arrivalTime = current_event->time; // Set Room inital time it requires cleaning

    // Add to Cumulative Number total, and set new time interval start
    cumulative_number += current_number_patients * (current_event->time - cumulative_number_time_interval_start);
    cumulative_number_time_interval_start = current_event->time;

    departure_count_stage2++;         // Increment total departures from system
    current_number_patients--; // Decrement total number of patients in system
    Room* r = listOfRooms->removeRoom(roomToClear);
    assert(roomToClear != NULL);
    assert(roomToClear == r);
    delete patientLeaving;

    eventL->insert(new EventNode(current_event->time, 3, 1, r));

    return;
}

void Cleaning_Arrival(Queue *CQ, EventList *eventL, int totalRooms, RoomList *listOfRooms, int totalJanitors)
{
    // Tasks:
    // - Schedule start service event
    // - Check if all janitors are busy, if not, start service for the rooms right away
    // - Add to cumulative num of patients

    assert(CQ != NULL); // Checks for NULL/incorrect parameter values
    assert(eventL != NULL);
    assert(current_event->type == 1);
    assert(countOfRoomsBeingUsed <= totalRooms);


    Room *room_in_event = current_event->room; // Get room for Stage 3

    total_arrival_times += room_in_event->arrivalTime;
    // room_in_event->arrival_time = current_event->time; //arrival time already set by stage 2 departure

    if (busy_janitors >= totalJanitors)
    { // If all janitors are busy, put in cleaning queue only
        // Puts rooms in Queue
        CQ->stage3_push(room_in_event);
    }
    else
    { // If not, put in Queue & schedule Start Service event
        // Schedule start service event
        CQ->stage3_push(room_in_event);
        eventL->insert(new EventNode(current_event->time, 3, 2, room_in_event));
    }

    cumulative_room_calls++; // needed for avg response time of cleaning up rooms
    return;
}

void Cleaning_StartServuce(Queue *CQ, EventList *eventL, RoomList *listOfRooms, double mu_clean, int totalJanitors)
{

    // Tasks:
    // - Schedule Departure
    // - Record waiting time of room
    // - Mark room being cleaned
    // - Add to cumulative num of patients

    assert(CQ != NULL); // Check for NULL/incorrect parameter values
    assert(eventL != NULL);
    assert(listOfRooms != NULL);
    assert(current_event->type == 2);

    Room *roomAssignedToJanitor = CQ->room_pop(); // Remove room from cleaning Queue
    assert(roomAssignedToJanitor != NULL);        // Room should not be null

    roomAssignedToJanitor->status = 3; // Set room to be "3" = "Cleaning"

    double service_time; // generate service time for room cleaning.
    double u = GenerateRandomNumber();
    if (mu_clean == 0)
    {
        service_time = 0;
    }
    else
    {
        service_time = -(log(1 - u) / mu_clean);
    }
    roomAssignedToJanitor->serviceTime = service_time;
    assert(service_time > 0);
    busy_janitors++;

    if (busy_janitors < totalJanitors && CQ->waiting_count > 0) {
        eventL->insert(new EventNode(current_event->time, 3, 2, CQ->room_head));
    }

    //     Insert Departure event into Event List
    eventL->insert(new EventNode((current_event->time + service_time), 3, 3, roomAssignedToJanitor));
    return;
}

void Cleaning_Departure(EventList *eventL, RoomList *listOfRooms, Queue *CQ, Queue *PQ)
{

    // Tasks:
    // - Set room to "available"
    // - update stats
    // - decrement busy rooms and janitors
    // - start service for one more room from the waiting list.

    assert(eventL != NULL); // Check for NULL/incorrect parameter values
    assert(listOfRooms != NULL);
    assert(current_event->type == 3);

    Room *roomToFree = current_event->room; // Fetches Room
    assert(roomToFree != NULL);
    roomToFree->status = 0; // Set Room to "available". only stage_2 uses the status check to find a empty/clean room.

    cumulative_response_cleanUp += current_event->time - roomToFree->arrivalTime;

    busy_janitors--;         // Decrement total busy janitors
    countOfRoomsBeingUsed--; // Decrement number of rooms being used
    if (CQ->waiting_count > 0)
    {                                                                            // waiting count will only be greater than 0 if there's a room that has yet to be serviced and all janitors were busy.
        eventL->insert(new EventNode(current_event->time, 3, 2, CQ->room_head)); // since a janitor is now free, it can take up another room to clean,
    }                                                                            // trigger start service for the head of the cleaning waiting queue.

    if (PQ->waiting_count > 0) {
        statementTrue++;                                                    // same concept as above, instead of room queue, need to check patient queue that are waiting for a empty room.
        eventL->insert(new EventNode(current_event->time, 2, 2, PQ->head)); // a room is now free, another patient can use the room.
    }                                                                       // trigger start service for the head of the patient waiting queue.

    listOfRooms->insert(roomToFree);
}

/* SIMULATION */
void Simulation(float lambda_low, float lambda_mid, float lambda_high,
                float mu_er, float mu_low, float mu_mid, float mu_high, float mu_clean,
                int B, int R, int m1, int m2)
{

    Queue *EQ = new Queue(); // For stage 1
    Queue *PQ = new Queue(); // For stage 2
    Queue *CQ = new Queue(); // For stage 3
    EventList *eventL = InitializeEventlist();

    assert(EQ != NULL);
    assert(PQ != NULL);
    assert(CQ != NULL);
    assert(eventL != NULL);

    // Create servers / array of patient pointers + array to keep track of departure times
    // Initialize all entries to NULL
    PatientNode *nurses[m1];
    double stage1_departure_times[m1];
    for (int a = 0; a < m1; a++)
    {
        nurses[a] = NULL;
        stage1_departure_times[a] = simulation_duration + 1;
    }

    // Initalize list of Rooms
    RoomList* listOfRooms = new RoomList();
    for (int i = 0; i < R; i++){
        // Create a function to create service times for rooms
        Room *newRoom = new Room(0, 0, 0);
        listOfRooms->insert(newRoom);
    }

    // Create first set of patients
    next_arrival_low = CreatePatient(0, 1, lambda_low, mu_er, mu_low);
    next_arrival_mid = CreatePatient(0, 2, lambda_mid, mu_er, mu_mid);
    next_arrival_high = CreatePatient(0, 3, lambda_high, mu_er, mu_high);

    next_arrival = threeMinArrival(next_arrival_low, next_arrival_mid, next_arrival_high);

    // Insert first arrival into event list
    eventL->insert(new EventNode(next_arrival->arrival_time, 1, 1, next_arrival));
    current_event = eventL->head;

    while (current_event->type != 10)
    {
        //    cout << next_arrival_low->arrival_time << " - " << next_arrival_mid->arrival_time << " - " << next_arrival_high->arrival_time << endl;
        //    cout << "T+" << current_event->time << "\tPatient: " << current_event->patient->arrival_time << "\tStage: " << current_event->stage << "\tStep: " << current_event->type << endl;
        //    cout << "Patients in system: "<< current_number_patients <<"\tEQ Waiting count: " << EQ->waiting_count <<"\tPQ Waiting count: " << PQ->waiting_count << "\tDepartures: " << departure_count_stage2 << endl;
        //    cout << "Current event: " << current_event->stage << " " << current_event->type << endl;

        assert(current_event != NULL);

        switch (current_event->stage)
        {
        case 1:
            switch (current_event->type)
            {
            case 1:
                Stage1_Arrival(EQ, eventL, B, nurses, m1, stage1_departure_times, lambda_low, lambda_mid, lambda_high, mu_er, mu_low, mu_mid, mu_high);
                break;
            case 2:
                Stage1_StartService(EQ, eventL, nurses, stage1_departure_times, m1);
                break;
            case 3:
                Stage1_Departure(EQ, eventL, nurses, stage1_departure_times, m1);
                break;
            }
            break;

        case 2:
            switch (current_event->type)
            {
            case 1:
                Stage2_Arrival(PQ, eventL, R, listOfRooms);
                break;
            case 2:
                Stage2_StartService(PQ, eventL, listOfRooms, CQ);
                break;
            case 3:
                Stage2_Departure(eventL, listOfRooms, PQ);
                break;
            }

            break;

        case 3:
            switch (current_event->type)
            {
            case 1:
                Cleaning_Arrival(CQ, eventL, R, listOfRooms, m2);
                break;
            case 2:
                Cleaning_StartServuce(CQ, eventL, listOfRooms, mu_clean, m2);
                break;
            case 3:
                Cleaning_Departure(eventL, listOfRooms, CQ, PQ);
                break;
            }

            break;
        }

        if (floor(current_event->next->time / print_period) > floor(current_event->time / print_period) && current_event->next->type != 10) {
            // Print statistics every print period
            PrintStatistics(EQ, PQ, CQ);
        }

        current_event = current_event->next;
        delete eventL->removeHead();
    }

    // End of Simulation
    assert(current_event->type == 10);

    // Print statistics at the end of simulation
    PrintStatistics(EQ, PQ, CQ);
    
    // Clean up
    for (int i = 0; i < m1; i++)
    {
        if (nurses[i] != NULL)
            delete nurses[i];
    }

    while (EQ->head != NULL){
        PatientNode* temp = EQ->pop();
        delete temp;
    }
    delete EQ;

    while (PQ->head != NULL){
        PatientNode* temp = PQ->pop();
        delete temp;
    }
    delete PQ;

    while (CQ->head != NULL){
        Room* temp = CQ->room_pop();
        delete temp;
    }
    delete CQ;
    delete next_arrival_high;
    delete next_arrival_mid;
    delete next_arrival_low;
    delete eventL;
}

int main(int argc, char *argv[])
{
    if (argc >= 14)
    {
        float lambda_high = atof(argv[1]);
        float lambda_mid = atof(argv[2]);
        float lambda_low = atof(argv[3]);
        float mu_er = atof(argv[4]);
        float mu_high = atof(argv[5]);
        float mu_mid = atof(argv[6]);
        float mu_low = atof(argv[7]);
        float mu_clean = atof(argv[8]);
        int B = atoi(argv[9]);
        int R = atoi(argv[10]);
        int m1 = atoi(argv[11]);
        int m2 = atoi(argv[12]);
        int seed = atoi(argv[13]);

        assert(lambda_high >= 0);
        assert(lambda_mid >= 0);
        assert(lambda_low >= 0);
        assert(mu_er >= 0);
        assert(mu_high >= 0);
        assert(mu_mid >= 0);
        assert(mu_low >= 0);
        assert(mu_clean >= 0);
        assert(B > 0);
        assert(R > 0);
        assert(m1 > 0);
        assert(m2 > 0);
        assert(seed > 0);

        cout << endl << "CMPT 305 Project 1: Hospital Emergency Department Simulation" << endl;
        cout << "lambda_h = " << lambda_high << ", lambda_m = " << lambda_mid << ", lambda_l = " << lambda_low << ", mu_e = " << mu_er << ", mu_h = " << mu_high << ", mu_m = " << mu_mid << ", mu_l = " << mu_low << ", mu_c = " << mu_clean << ", B = " << B << ", R = " << R << ", m1 = " << m1 << ", m2 = " << m2 << ", S = " << seed << endl << endl;
        srand(seed);    
        Simulation(lambda_low, lambda_mid, lambda_high, mu_er, mu_low, mu_mid, mu_high, mu_clean, B, R, m1, m2);
    }
    else
        cout << "Insufficient number of arguments provided!" << endl;

    return 0;
}