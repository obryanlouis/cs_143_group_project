// Control.cpp

#include <iostream>

#include "Control.h"

/* Controller functions */
Controller::Controller(){
    this->schedule_p = new Scheduler();
    // TODO: Update the number of flows. 
    this->flowsLeft = 0;
}

void Controller::run(){
    bool noError = true;
    while (!noError && this->flowsLeft != 0) {
        noError = this->schedule_p->doNext();
    }

    if (!noError){
        std::cout << "ERROR HAS OCCURRED. ABORT." << std::endl;
        exit(1);
    }

    if (this -> flowsLeft == 0) {
        for (int i = 0; i < END_PERIOD; i++){
            this->schedule_p->doNext();
        }
    }

    std::cout << "Network simulated successfully" << std::endl;
}


/* Scheduler functions */
/* Initiate scheduler by creating schedule queue and putting in initial events. */ 
Scheduler::Scheduler(){
    this->events_p = new std::priority_queue<Event*, std::vector<Event*>, timecomp>; 
    add(UPDATE_ROUTING, 0);
    add(PRINT_STATS,0);
}

Scheduler::~Scheduler(){
    delete[] this->events_p;
}

void Scheduler::add(Event* event_p){
    this->events_p->push(event_p);
    std::cout << "pushed event of type " << event_p->getType() << std::endl;
}

void Scheduler::add(event_t in_type, unsigned int in_time, void *in_actOn){
    Event *new_event = new Event(in_type, in_time, in_actOn);
    this->events_p->push(new_event);
    std::cout << "made/pushed event of type " << new_event->getType() << std::endl;
}

/* Do next event in the event queue. If this event causes another event to be added
 * to the event queue, do so here. 
 */
bool Scheduler::doNext(){
    if (this->events_p->size() != 0){
        Event *new_event = this->events_p->top();
        Event *next_event = new_event->execute();
        this->events_p->pop();

        if (next_event) {
            add(next_event);
        }

        return true;
    }

    std::cout << "Error: Schedule queue is empty!" << std::endl;
    return false;
}

void Scheduler::printAndDestroySchedule(){
    while (this->events_p->size() != 0){
        Event* curr = this->events_p->top();
        this->events_p->pop();
        std::cout << "Time " << curr->getTime() << "| Type = " << curr->getType() << \
            std::endl;
    }
}

/* Event functions */

// Execute function should be kept up to date with the event type struct.
Event *Event::execute(){
    if (type == UPDATE_ROUTING){
        std::cout << "Signal for router update sent." << std::endl;
        // TODO: add functions to call for routing event
        Event *new_event = new Event(UPDATE_ROUTING, this->time + ROUTING_UPDATE_PERIOD);
        return new_event;
    }
    else if (type == PRINT_STATS){
        std::cout << "Printing system stats to files." << std::endl;
    // TODO
        Event *new_event = new Event(PRINT_STATS, this->time + SNAPSHOT_PERIOD);
        return new_event;
    }
    else if (type == UPDATE_FLOWS){
        std::cout << "Updating flow " << actOn << std::endl;
    // TODO
    }
    else if (type == UPDATE_LINKS){
        std::cout << "Updating link " << actOn << std::endl; 
    }
   
    else {
        std::cout << "Error: event type " << type << " is not valid." << std::endl;
        std::cout << "Exiting program." << std::endl;
        exit(1); 
    }

}
