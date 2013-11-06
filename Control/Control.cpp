// Control.cpp

#include "Control.h"

/* Controller functions */
Controller::Controller(){
    SYSTEM_CONTROLLER = this; 
    this->schedule_p = new Scheduler();
    this->schedule_p->initScheduler();
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

    std::cout << "Network simulated successfully. YAY!" << std::endl;
}

void Controller::routerUpdate(){
    std::cout << "***Updating Router Info*** " << std::endl;
    // TODO: Send router update information.
}

void Controller::printSystem() {
    std::cout << "***Printing System***" <<std::endl;
    unsigned int currentTime = this->schedule_p->getCurrentTime();
    std::map<std::string, std::ofstream*> files;
    // Links
/*    std::cout << "  Outputting Link information." << std::endl;
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
               // TODO: Make sure that the snapshot period thing matches units
               int timeDifference = SNAPSHOT_PERIOD;
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
*/    
    // TODO: Routers, Hosts, Flows
    std::cout << "  Outputting Flow information." << std::endl;
    
    std::cout << "--Done printing system." << std::endl;
}

void Controller::add(Event *event_p) {
    schedule_p->add(event_p);
}

unsigned int Controller::getCurrentTime() {
    return SYSTEM_TIME;
}

void routerUpdate(void* args){
    SYSTEM_CONTROLLER->routerUpdate();
    void (*fp)(void*) = &routerUpdate;
    SYSTEM_CONTROLLER->add  \
        (new Event(SYSTEM_TIME + ROUTING_UPDATE_PERIOD, fp, 0));
}

void printSystem(void* args){
    SYSTEM_CONTROLLER->printSystem();
    void (*fp)(void*) = &printSystem;
    SYSTEM_CONTROLLER->add \
        (new Event(SYSTEM_TIME + SNAPSHOT_PERIOD, fp, 0));
}

/* Scheduler functions */
/* Initiate scheduler by creating schedule queue and putting in initial events. */ 
Scheduler::Scheduler(){
    events_p = new std::priority_queue<Event*, std::vector<Event*>, timecomp>; 
    SYSTEM_TIME = 0;
}

void Scheduler::initScheduler(){
    // TODO: make callback functions for the routing update and the stat printing
    void (*fp)(void*) = &routerUpdate;
    Event *event = new Event(0, fp, 0);
    SYSTEM_CONTROLLER->add(event);
    fp = &printSystem;
    event = new Event(0, fp, 0);
    SYSTEM_CONTROLLER->add(event);
}

Scheduler::~Scheduler(){
    delete[] events_p;
}

void Scheduler::add(Event* event_p){
    events_p->push(event_p);
}

bool Scheduler::doNext(){
    if (this->events_p->size() != 0){
        Event *new_event = this->events_p->top();

        // Update the current time of the scheduler and execute event.
        SYSTEM_TIME = new_event->getTime();
        new_event->execute();

        delete new_event; 
        this->events_p->pop();

        return true;
    }

    std::cout << "Error: Schedule queue is empty!" << std::endl;
    return false;
}

void Scheduler::printAndDestroySchedule(){
    while (this->events_p->size() != 0){
        Event* curr = this->events_p->top();
        this->events_p->pop();
        std::cout << "Time " << curr->getTime() << std::endl;
    }
}

unsigned int Scheduler::getCurrentTime() {
    return SYSTEM_TIME;
}

/* Event functions */


void Event::execute(){
    this->fp(this->arg);
}
