// Control.cpp

#include "Control.h"

/* Pointer to the master controller. 
 * This is ONLY set by the Controller constructor. 
 */
// This can't be static...
Controller *SYSTEM_CONTROLLER;

extern void makePlots(); 

/* Controller functions */
Controller::Controller(){
    SYSTEM_CONTROLLER = this;

    this->routers_p = new std::list<Router*>();
    this->links_p = new std::list<Link*>();
    this->flows_p = new std::list<Flow*>();
    this->hosts_p = new std::list<Host*>();

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

void Controller::addHost(Host *host) {
    this->hosts_p->push_back(host);
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

    makePlots();

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

void Controller::decrementFlowsLeft() {
    this->flowsLeft--;
}

void Controller::printMySystem() {
    std::cout << "***Printing System***" <<std::endl;
    unsigned int currentTime = this->schedule_p->getCurrentTime();
    std::map<std::string, std::ofstream*> files;
    // Links
    std::cout << "  Outputting Link information." << std::endl;
    std::ofstream occupancyFile, lossFile, rateFile;
    occupancyFile.open(LINK_OCCUPANCY_FILE.data(), std::ios::app);
    lossFile.open(LINK_PACKET_LOSS_FILE.data(), std::ios::app);
    rateFile.open(LINK_FLOW_RATE_FILE.data(), std::ios::app);
    files["occupancy"] = &occupancyFile;
    files["loss"] = &lossFile;
    files["data sent"] = &rateFile;
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
               (*file) << " " << link->getStat(stat, SNAPSHOT_PERIOD);
        }
        (*file) << "\n";
        file->close();
    }
    //  Reset the stats
    for (std::list<Link*>::iterator it = this->links_p->begin();
         it != this->links_p->end(); it++)
    {
           (*it)->resetStats();
    }

    std::cout << "  Outputting Flow information." << std::endl;
    std::ofstream flowSendFile, flowReceiveFile, flowRTTFile;
    flowSendFile.open(FLOW_SEND_FILE.data(), std::ios::app);
    flowReceiveFile.open(FLOW_RECEIVE_FILE.data(), std::ios::app);
    flowRTTFile.open(FLOW_RTT_FILE.data(), std::ios::app);
    files.clear();
    files["send rate"] = &flowSendFile;
    files["receive rate"] = &flowReceiveFile;
    files["rtt"] = &flowRTTFile;
    for (std::map<std::string, std::ofstream*>::iterator i = files.begin();
         i != files.end(); i++)
    {
        std::ofstream *file = i->second;
        std::string stat = i->first;
        (*file) << currentTime;
        for (std::list<Flow*>::iterator it = this->flows_p->begin();
             it != this->flows_p->end(); it++)
        {
               Flow *flow = *it;
               (*file) << " " << flow->getStats(stat, SNAPSHOT_PERIOD);
        }
        (*file) << "\n";
        file->close();
    }

    //  Reset the stats
    for (std::list<Flow*>::iterator it = this->flows_p->begin();
         it != this->flows_p->end(); it++)
    {
           (*it)->resetStats();
    }

    std::cout << "Outputting Host information." << "\n";
    files.clear();
    std::ofstream hostSendFile, hostReceiveFile;
    hostSendFile.open(HOST_SEND_FILE.data(), std::ios::app);
    hostReceiveFile.open(HOST_RECEIVE_FILE.data(), std::ios::app);
    files["send rate"] = &hostSendFile;
    files["receive rate"] = &hostReceiveFile;
    for (std::map<std::string, std::ofstream*>::iterator i = files.begin();
         i != files.end(); i++)
    {
        std::ofstream *file = i->second;
        std::string stat = i->first;
        (*file) << currentTime;
        for (std::list<Host*>::iterator it = this->hosts_p->begin();
             it != this->hosts_p->end(); it++)
        {
               Host *host = *it;
               (*file) << " " << host->getStats(stat, SNAPSHOT_PERIOD);
        }
        (*file) << "\n";
        file->close();
    }
    //  Reset the stats
    for (std::list<Host*>::iterator it = this->hosts_p->begin();
         it != this->hosts_p->end(); it++)
    {
           (*it)->resetStats();
    }

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

void removeOldStatsFiles() {
    std::list<std::string> filenames;
    filenames.push_back(LINK_OCCUPANCY_FILE);
    filenames.push_back(LINK_PACKET_LOSS_FILE);
    filenames.push_back(LINK_FLOW_RATE_FILE);
    filenames.push_back(FLOW_SEND_FILE);
    filenames.push_back(FLOW_RECEIVE_FILE);
    filenames.push_back(FLOW_RTT_FILE);
    filenames.push_back(HOST_SEND_FILE);
    filenames.push_back(HOST_RECEIVE_FILE);

    for (std::list<std::string>::iterator it = filenames.begin();
        it != filenames.end(); it++)
    {
        std::ifstream ifile(it->data());
        // If the file exists, delete it
        if (ifile) {
            if (remove(it->data()) != 0) {
                std::cout << "Could not delete file " + *it;
                exit(1);
            }
        }
    }
}

void Controller::initSystem(){
    // Init flows
    this->flowsLeft = flows_p->size();

    // Delete old stats files
    removeOldStatsFiles();

    // Do first router update/system print
    void *args;
    routerUpdate(args);
    printSystem(args);
}

void Controller::addPacket(Packet *p) {
    packets[p] = 1;
}
void Controller::removePacket(Packet *p) {
    packets.erase(p);
}
void Controller::checkPackets() {
    std::cout << "Total Packets in system: " << packets.size() << "\n";
    for (std::map<Packet *, bool>::iterator it = packets.begin();
        it != packets.end(); it++)
    {
        Packet *packet = it->first;
        if (packet == NULL) {
            std::cout << "Controller::checkPackets : NULL PACKET\n";
            exit(1);
        }
        assertPacketExists(packet);
        if (packet->getSource() == NULL) {
            std::cout << "Controller::checkPackets : Packet source null\n";
            exit(1);
        }
        int type = packet->getType();
        if (type < 0 || type > 3) {
            std::cout << "Controller::checkPackets : INVALID PACKET TYPE\n";
            exit(1);
        }
    }
}

void Controller::assertPacketExists(Packet *p) {
    if (!packets[p]) {
        std::cout << "Packet does not exist.\n";
        exit(1);
    }
}

void Controller::assertNodeExists(Node *n) {
    if (std::find(hosts_p->begin(), hosts_p->end(), n) == hosts_p->end() &&
        std::find(routers_p->begin(), routers_p->end(), n) == routers_p->end())
    {
        std::cout << "Node does not exist.\n";
        exit(1);
    }
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

        // DEBUG: Check that all packets are valid
        SYSTEM_CONTROLLER->checkPackets();

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

