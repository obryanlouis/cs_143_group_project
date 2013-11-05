// Control.cpp

#include "Control.h"

/* Controller functions */
Controller::Controller(){
    SYSTEM_CONTROLLER = this; 
    this->schedule_p = new Scheduler();
    // TODO: Update the number of flows. 
    this->flowsLeft = 0;
    this->prevStatCollectTime = 0;
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

    std::cout << "Network simulated successfully. YAY!" << std::endl;
}

void Controller::routerUpdate(){
    std::cout << "  " << std::endl;
    // TODO: Send router update information.
}

void Controller::printSystem() {
    unsigned int currentTime = this->schedule_p->getCurrentTime();
    std::map<std::string, std::ofstream*> files;
    // Links
    std::cout << "  Outputting Link information." << std::endl;
    std::ofstream *occupancyFile, *lossFile, *rateFile;
    occupancyFile->open(LINK_OCCUPANCY_FILE.data(), std::ios::app);
    lossFile->open(LINK_PACKET_LOSS_FILE.data(), std::ios::app);
    rateFile->open(LINK_FLOW_RATE_FILE.data(), std::ios::app);
    files["occupancy"] = occupancyFile;
    files["loss"] = lossFile;
    files["data sent"] = rateFile;
    for (std::map<std::string, std::ofstream*>::iterator i = files.begin();
         i != files.end(); i++)
    {
        std::ofstream *file = i->second;
        std::string stat = i->first;
        (*file) << currentTime;
        for (std::list<Link*>::iterator it = this->links_p->begin();
             it != this->links_p->end(); it++)
        {
               Link *link = *it;
            // Calculate the flow rate during this time interval
            if (stat.compare("data sent") == 0) {
               int dataSent = link->getStat(stat);
               int timeDifference = currentTime - this->prevStatCollectTime;
               (*file) << " " << (double)dataSent / (double)timeDifference;
            }
            // For the values that can be directly saved do so
            else {
               (*file) << " " + link->getStat(stat);
            }
        }
        (*file) << "\n";
        file->close();
    }
    
    // TODO: Routers, Hosts, Flows
    std::cout << "  Outputting Flow information." << std::endl;
    
    std::cout << "--Done printing system." << std::endl;
    this->prevStatCollectTime = currentTime;
}

void Controller::add(Event *event_p) {
    this->schedule_p->add(event_p);
}

unsigned int Controller::getCurrentTime() {
    return this->schedule_p->getCurrentTime();
}

/* Scheduler functions */
/* Initiate scheduler by creating schedule queue and putting in initial events. */ 
Scheduler::Scheduler(){
    this->events_p = new std::priority_queue<Event*, std::vector<Event*>, timecomp>; 
    // TODO: make callback functions for the routing update and the stat printing
    //add(UPDATE_ROUTING, 0);
    //add(PRINT_STATS,0);
    this->currentTime = 0;
}

Scheduler::~Scheduler(){
    delete[] this->events_p;
}

void Scheduler::add(Event* event_p){
    this->events_p->push(event_p);
    //std::cout << "pushed event of type " << event_p->getType() << std::endl;
}

/*void Scheduler::add(event_t in_type, unsigned int in_time, void *in_actOn){
    Event *new_event = new Event(in_type, in_time, in_actOn);
    this->events_p->push(new_event);
    //std::cout << "made and pushed event of type " << new_event->getType() << std::endl;
}*/

/* Do next event in the event queue. If this event causes another event to be added
 * to the event queue, do so here. 
 */
bool Scheduler::doNext(){
    if (this->events_p->size() != 0){
        Event *new_event = this->events_p->top();
        // Update the current time of the scheduler.
        this->currentTime = new_event->getTime();
        Event *next_event = new_event->execute();

        delete new_event; 
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
        //std::cout << "Time " << curr->getTime() << "| Type = " << curr->getType() << \
            std::endl;
    }
}

unsigned int Scheduler::getCurrentTime() {
    return this->currentTime;
}

/* Event functions */

// Execute function should be kept up to date with the event type struct.
Event *Event::execute(){
    // Execute the pointer to the callback function with the given argument
    this->fp(this->arg);
    return NULL;
    /*if (type == UPDATE_ROUTING){
        std::cout << "Signal for router update sent." << std::endl;
        SYSTEM_CONTROLLER->routerUpdate();
        Event *new_event = new Event(UPDATE_ROUTING, this->time + ROUTING_UPDATE_PERIOD);
        return new_event;
    }
    else if (type == PRINT_STATS){
        std::cout << "Printing system stats to files." << std::endl;
        SYSTEM_CONTROLLER->printSystem();
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
    }*/

}
