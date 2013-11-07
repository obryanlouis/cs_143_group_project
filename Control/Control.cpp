// Control.cpp

#include "Control.h"

/* Pointer to the master controller. 
 * This is ONLY set by the Controller constructor. 
 */
// This can't be static...
Controller *SYSTEM_CONTROLLER;

/* Controller functions */
Controller::Controller(){
    SYSTEM_CONTROLLER = this;

    this->routers_p = new std::list<Router*>();
    this->links_p = new std::list<Link*>();
    this->flows_p = new std::list<Flow*>();

    this->schedule_p = new Scheduler();
}

Controller::~Controller(){
    delete this->routers_p;
    delete this->links_p;
    delete this->flows_p;
    delete this->schedule_p;
}

void Controller::addRouter(Router *router){
    this->routers_p->push_back(router);
}

void Controller::addLink(Link *link){
    this->links_p->push_back(link);
}


extern void maintainFlowCallback(void *arg);

void Controller::addFlow(Flow *flow, unsigned int startTime){
    this->flows_p->push_back(flow);

    // Create first flow event for this flow 
    Event *e = new Event(startTime, &maintainFlowCallback, flow);
    this->add(e);


}

unsigned int Controller::getCurrentTime() {
    return SYSTEM_TIME;
}

void Controller::run(){
    this->initSystem();

    bool noError = true;
    while (noError && this->flowsLeft != 0) {
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

void Controller::updateMyRouters(){
    std::cout << "***Updating Router Info*** " << std::endl;

    for (std::list<Router *>::iterator it = this->routers_p->begin();
         it != this->routers_p->end(); it++)
    {
        (*it)->broadcastRoutingTable();
    }
}

void Controller::printMySystem() {
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

void routerUpdate(void* args){
    SYSTEM_CONTROLLER->updateMyRouters();
    void (*fp)(void*) = &routerUpdate;
    SYSTEM_CONTROLLER->add  \
        (new Event(SYSTEM_TIME + ROUTING_UPDATE_PERIOD, fp, 0));
}

void printSystem(void* args){
    SYSTEM_CONTROLLER->printMySystem();
    void (*fp)(void*) = &printSystem;
    SYSTEM_CONTROLLER->add \
        (new Event(SYSTEM_TIME + SNAPSHOT_PERIOD, fp, 0));
}

void Controller::initSystem(){
    // Init flows
    this->flowsLeft = flows_p->size();
    std::cout << "Number of input flows: " << flowsLeft << "\n";

    // Do first router update/system print
    void *args;
    routerUpdate(args);
    printSystem(args);
}


/* Scheduler functions */
/* Initiate scheduler by creating schedule queue and putting in initial events. */ 
Scheduler::Scheduler(){
    events_p = new std::priority_queue<Event*, std::vector<Event*>, timecomp>; 
    SYSTEM_TIME = 0;
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
