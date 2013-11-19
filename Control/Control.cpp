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

    this->schedule_p = new Scheduler();
}

Controller::~Controller(){
    // delete all routers, links, flows, and hosts in the system 
  /*  while(!this->routers.empty()){
         delete this->routers.front();
         this->routers.pop_front();
    }
    while(!this->links.empty()){
         std::cout << links.front();
         delete this->links.front();
         this->links.pop_front();
    }
    while(!this->flows.empty()){
         delete this->flows.front();
         this->flows.pop_front();
    }
    while(!this->hosts.empty()){
         delete this->hosts.front();
         this->hosts.pop_front();
    }
    */
}

void Controller::addRouter(Router *router){
    this->routers.push_back(router);
}

void Controller::addLink(Link *link){
    this->links.push_back(link);
}

void Controller::addHost(Host *host) {
    this->hosts.push_back(host);
}

extern void maintainFlowCallback(void *arg);

void Controller::addFlow(Flow *flow, unsigned int startTime){
    this->flows.push_back(flow);

    // Create first flow event for this flow 
    Event *e = new Event(startTime, &maintainFlowCallback, flow);
    this->add(e);


}

unsigned int Controller::getCurrentTime() {
    return SYSTEM_TIME;
}

void Controller::run(std::string inputFile){
    this->initSystem(inputFile);
    ROUTING_UPDATE_PERIOD = this->routingUpdateTime;
    SNAPSHOT_PERIOD       = this->snapshotTime;

    bool noError = true;
    while (noError && this->flowsLeft != 0) {
        noError = this->schedule_p->doNext();
        printRoutingTables();
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

    for (std::list<Router *>::iterator it = this->routers.begin();
         it != this->routers.end(); it++)
    {
        (*it)->broadcastRoutingTable();
    }

    // DEBUG
    printRoutingTables();
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
        for (std::list<Link*>::iterator it = this->links.begin();
             it != this->links.end(); it++)
        {
               Link *link = *it;
               (*file) << " " << link->getStat(stat, SNAPSHOT_PERIOD);
        }
        (*file) << "\n";
        file->close();
    }
    //  Reset the stats
    for (std::list<Link*>::iterator it = this->links.begin();
         it != this->links.end(); it++)
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
        for (std::list<Flow*>::iterator it = this->flows.begin();
             it != this->flows.end(); it++)
        {
               Flow *flow = *it;
               (*file) << " " << flow->getStats(stat, SNAPSHOT_PERIOD);
        }
        (*file) << "\n";
        file->close();
    }

    //  Reset the stats
    for (std::list<Flow*>::iterator it = this->flows.begin();
         it != this->flows.end(); it++)
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
        for (std::list<Host*>::iterator it = this->hosts.begin();
             it != this->hosts.end(); it++)
        {
               Host *host = *it;
               (*file) << " " << host->getStats(stat, SNAPSHOT_PERIOD);
        }
        (*file) << "\n";
        file->close();
    }
    //  Reset the stats
    for (std::list<Host*>::iterator it = this->hosts.begin();
         it != this->hosts.end(); it++)
    {
           (*it)->resetStats();
    }

    std::cout << "--Done printing system." << std::endl;
}

void Controller::add(Event *event_p) {
    schedule_p->add(event_p);
}

void Controller::setSnapshotTime(int t) {
    this->snapshotTime = t;
}

void Controller::setRoutingUpdateTime(int t) {
    this->routingUpdateTime = t;
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

Node * Controller::getNode(int type, int id, std::map<int, Host* > hostsById,
        std::map<int, Router* > routersById)
{
    if (type == 0) {
        if (hostsById.find(id) != hostsById.end()) {
            return hostsById[id];
        }
        else {
            std::cout << "Host Id in input not found: " << id << "\n";
            exit(1);
        }
    }
    else if (type == 1) {
        if (routersById.find(id) != routersById.end()) {
            return routersById[id];
        }
        else {
            std::cout << "Router Id in input not found: " << id << "\n";
            exit(1);
        }
    }
    std::cout << "Invalid node type: " << type << "\n";
    exit(1);
}

void Controller::initSystem(std::string inputFile){
    // Run the parser
    InputParser parser(inputFile);
    parser.run(
            this->snapshotTime,
            this->routingUpdateTime,
            this->hostInfos,
            this->routerInfos,
            this->linkInfos,
            this->flowInfos);

    // Create the objects
    for (std::list<HostInfo>::iterator it = hostInfos.begin(); it != hostInfos.end(); it++)
    {
        Host *h = new Host(it->hostId);
        hostsById[it->hostId] = h;
        SYSTEM_CONTROLLER->addHost(h);
    }
 
    for (std::list<FlowInfo>::iterator it = flowInfos.begin();
         it != flowInfos.end(); it++)
    {
        Flow *f = new Flow(it->flowId, it->flowSize,
                hostsById[it->sourceId], hostsById[it->destinationId]);
        SYSTEM_CONTROLLER->addFlow(f, 1000 * it->startTime);
    }

    for (std::list<RouterInfo>::iterator it = routerInfos.begin();
         it != routerInfos.end(); it++)
    {
        Router *r = new Router(it->routerId);
        routersById[it->routerId] = r;
        SYSTEM_CONTROLLER->addRouter(r);
    }

    for (std::list<LinkInfo>::iterator it = linkInfos.begin();
         it != linkInfos.end(); it++)
    {
        Node *n1 = getNode(it->node1Type, it->node1Id,
                hostsById, routersById);
        Node *n2 = getNode(it->node2Type, it->node2Id,
                hostsById, routersById);
        Link *l = new Link(it->linkId, n1, n2, it->bufferSize, it->linkRate,
                it->linkDelay);
        SYSTEM_CONTROLLER->addLink(l);
    }


    // Init flows
    this->flowsLeft = flows.size();

    // Delete old stats files
    removeOldStatsFiles();

    // Do first router update/system print
    void *args;
    routerUpdate(args);
    printSystem(args);
}

void checkPacketSource(Packet *p) {
    if (p->getSource() == NULL) {
        std::cout << "Packet source null\n";
        exit(1);
    }
}

int Controller::numTotalPackets() {
    return packets.size();
}

void Controller::addPacket(Packet *p) {
    checkPacketSource(p);
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
        int type = packet->getType();
        if (type < 0 || type > 3) {
            std::cout << "Controller::checkPackets : INVALID PACKET TYPE\n";
            exit(1);
        }
        checkPacketSource(packet);
        DataPacket *dp = (DataPacket *)packet;
        if (type == Packet::DATA || type == Packet::ACK) {
            if (dp->getId() < 0) {
                std::cout << "Negative packet id (" <<
                    dp->getId() << ")\n";
                exit(1);
            }
            if (dp->getId() > 10000) {
                std::cout << "Packet id ("
                          << dp->getId() 
                          << ") pretty large. It's probably wrong\n";
                exit(1);
            }
        }
    }
}

void Controller::assertPacketExists(Packet *p) {
    if (!packets[p]) {
        std::cout << "Packet at " << p << " does not exist.\n";
        exit(1);
    }
}

void Controller::assertNodeExists(Node *n) {
    if (std::find(hosts.begin(), hosts.end(), n) == hosts.end() &&
        std::find(routers.begin(), routers.end(), n) == routers.end())
    {
        std::cout << "Node does not exist.\n";
        exit(1);
    }
}

void Controller::printRoutingTables() {
    for (std::list<Router* >::iterator it = routers.begin();
            it != routers.end(); it++)
    {
        (*it)->print(); 
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

