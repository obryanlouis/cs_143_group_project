// Control.cpp

#include "Control.h"

// Pointer to the master controller. 
// This is ONLY set by the Controller constructor. 
// This can't be static...
Controller *SYSTEM_CONTROLLER;

extern void makePlots(PlotOptions *options); 
void routerUpdate(void* args);
void printSystem(void* args);

// Controller functions

Controller::Controller() {
    SYSTEM_CONTROLLER = this;

    this->schedule_p = new Scheduler();
}

Controller::~Controller()
{}

void Controller::addRouter(Router *router) {
    this->routers.push_back(router);
}

void Controller::addLink(Link *link) {
    this->links.push_back(link);
}

void Controller::addHost(Host *host) {
    this->hosts.push_back(host);
}

void Controller::addFlow(Flow *flow, double startTime) {
    this->flows.push_back(flow);

    // Create first flow event for this flow 
    flow->startFlow(startTime);
}

int Controller::numLinksToPrint() {
    int num = 0;
    for (std::list<LinkInfo>::iterator it = linkInfos.begin();
            it != linkInfos.end(); it++)
    {
        if (it->print)
            num++;
    }
    return num;
}

int Controller::numHostsToPrint() {
    int num = 0;
    for (std::list<HostInfo>::iterator it = hostInfos.begin();
            it != hostInfos.end(); it++)
    {
        if (it->print)
            num++;
    }
    return num;
}

int Controller::numFlowsToPrint() {
    int num = 0;
    for (std::list<FlowInfo>::iterator it = flowInfos.begin();
            it != flowInfos.end(); it++)
    {
        if (it->print)
            num++;
    }
    return num;
}

double Controller::getCurrentTime() {
    return SYSTEM_TIME;
}

void Controller::setInputFile(std::string inputFile) {
    this->inputFile = inputFile;
}

void Controller::run() {
    this->initSystem();

    // Do first router update/system print
    void *args;
    routerUpdate(args);
    printSystem(args);

    bool noError = true;
    while (noError && this->flowsLeft != 0) {
        noError = this->schedule_p->doNext();

        // DEBUG
        // printRoutingTables();
    }

    if (!noError) {
        std::cout << "ERROR HAS OCCURRED. ABORT." << std::endl;
        exit(1);
    }

    /*if (this -> flowsLeft == 0) {
      for (int i = 0; i < END_PERIOD; i++) {
      this->schedule_p->doNext();
      }
      }*/

    makePlots(&this->plotOptions);

    std::cout << "Network simulated successfully. YAY!" << std::endl;
}

void Controller::updateMyRouters() {
    std::cout << "***Updating Router Info*** " << std::endl;
    for (std::list<Link *>::iterator it = this->links.begin();
            it != this->links.end(); it++) {
        (*it)->setInstantaneousOccupancy();
    }

    for (std::list<Router *>::iterator it = this->routers.begin();
            it != this->routers.end(); it++)
    {
        (*it)->broadcastRoutingTable();
    }
}

void Controller::decrementFlowsLeft() {
    this->flowsLeft--;
}

void Controller::printMySystem() {


    //std::cout << "***Printing System***" <<std::endl;
    double currentTime = this->schedule_p->getCurrentTime();
    std::map<std::string, std::ofstream*> files;
    // Links
    //std::cout << "  Outputting Link information." << std::endl;
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
        (*file) << (double)currentTime / 1000;
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

    //std::cout << "  Outputting Flow information." << std::endl;
    std::ofstream flowSendFile, flowReceiveFile, flowRTTFile, flowWindowFile,
        /*flowThreshFile,*/ flowOutstandingFile, /*flowRenoFile, flowVegasFile,*/
        flowDelayFile;
    flowSendFile.open(FLOW_SEND_FILE.data(), std::ios::app);
    flowReceiveFile.open(FLOW_RECEIVE_FILE.data(), std::ios::app);
    flowRTTFile.open(FLOW_RTT_FILE.data(), std::ios::app);
    flowWindowFile.open(FLOW_WINDOW_FILE.data(), std::ios::app);
    //flowThreshFile.open(FLOW_THRESH_FILE.data(), std::ios::app);
    flowOutstandingFile.open(FLOW_OUTSTANDING_FILE.data(), std::ios::app);
    //flowRenoFile.open(FLOW_RENO_FILE, std::ios::app);
    //flowVegasFile.open(FLOW_VEGAS_FILE, std::ios::app);
    flowDelayFile.open(FLOW_DELAY_FILE, std::ios::app);
    files.clear();
    files["send rate"] = &flowSendFile;
    files["receive rate"] = &flowReceiveFile;
    files["delay"] = &flowDelayFile;
    files["rtt"] = &flowRTTFile;
    files["window"] = &flowWindowFile;
    //files["reno"] = &flowRenoFile;
    //files["vegas"] = &flowVegasFile;
    files["outstanding packets"] = &flowOutstandingFile;
    /*files["thresh"] = &flowThreshFile;
      files["outstanding"] = &flowOutstandingFile;*/
    for (std::map<std::string, std::ofstream*>::iterator i = files.begin();
            i != files.end(); i++)
    {
        std::ofstream *file = i->second;
        std::string stat = i->first;
        (*file) << (double)currentTime / 1000;
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

    //std::cout << "Outputting Host information." << "\n";
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
        (*file) << (double)currentTime / 1000;
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

    //std::cout << "--Done printing system." << std::endl;
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

void outputRoutingTables(void * args) {
    // A debug function

    std::ofstream file;
    file.open("routers.txt", std::ios::app);
    for (std::list<Router* >::iterator iter =
            SYSTEM_CONTROLLER->routers.begin();
            iter != SYSTEM_CONTROLLER->routers.end(); iter++)
    {
        file      << "Routing table print at time "
            << SYSTEM_CONTROLLER->getCurrentTime()
            << " " << (*iter)->infoString()
            << "\n";
        std::map<Node*, std::pair<double, Link*> > mapping = 
            (*iter)->routingTable_p->mapping;
        for (std::map<Node*, std::pair<double, Link*> >::iterator it =
                mapping.begin();
                it != mapping.end(); it++)
        {
            if (it->first != NULL && it->second.second != NULL) {
                file << "Distance to Node " << it->first->infoString()
                    << " is " << it->second.first << " via " 
                    << it->second.second->infoString()
                    << "\n";
            }
        }
    }
    file << "\n\n\n";
    file.close();
}

void routerUpdate(void* args) {
    SYSTEM_CONTROLLER->updateMyRouters();
    void (*fp)(void*) = &routerUpdate;
    SYSTEM_CONTROLLER->add  \
        (new Event(SYSTEM_TIME + ROUTING_UPDATE_PERIOD, fp, 0));
}

void printSystem(void* args) {
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
    filenames.push_back(FLOW_WINDOW_FILE);
    filenames.push_back(HOST_SEND_FILE);
    filenames.push_back(HOST_RECEIVE_FILE);
    filenames.push_back(FLOW_THRESH_FILE);
    filenames.push_back(FLOW_OUTSTANDING_FILE);
    filenames.push_back(FLOW_VEGAS_FILE);
    filenames.push_back(FLOW_DELAY_FILE);

    filenames.push_back("routers.txt");

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

void Controller::initSystem() {
    // Run the parser
    InputParser parser(this->inputFile);
    parser.run(
            this->snapshotTime,
            this->routingUpdateTime,
            this->hostInfos,
            this->routerInfos,
            this->linkInfos,
            this->flowInfos,
            this->plotOptions);

    ROUTING_UPDATE_PERIOD = this->routingUpdateTime;
    SNAPSHOT_PERIOD       = this->snapshotTime;

    // Create the objects
    for (std::list<HostInfo>::iterator it = hostInfos.begin();
            it != hostInfos.end(); it++)
    {
        Host *h = new Host(it->hostId);
        hostsById[it->hostId] = h;
        SYSTEM_CONTROLLER->addHost(h);
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
        Link *l = new Link(it->linkId, n1, n2, 1000 * it->bufferSize,
                it->linkDelay, 1000000 * it->linkRate / 8);
        SYSTEM_CONTROLLER->addLink(l);
    }

    for (std::list<FlowInfo>::iterator it = flowInfos.begin();
            it != flowInfos.end(); it++)
    {
        Flow *f = new Flow(it->flowId, 1000000 * it->flowSize,
                hostsById[it->sourceId], hostsById[it->destinationId],
                it->congestionAlgorithmType);
        SYSTEM_CONTROLLER->addFlow(f, 1000 * it->startTime);
    }

    // Init flows
    this->flowsLeft = flows.size();

    // Delete old stats files
    removeOldStatsFiles();
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

int Controller::numberOfPacketsInSystem() {
    return packets.size();
}

double Controller::routerBufferSize() {
    LinkInfo l = linkInfos.front();
    return l.linkRate * 1000 / (double) Packet::DATASIZE;
}

void Controller::checkPackets() {
    //std::cout << "Total Packets in system: " << packets.size() << "\n";
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
        std::cout << "Routing table print at time "
            << SYSTEM_CONTROLLER->getCurrentTime()
            << "\n";
        (*it)->print(); 
    }
}

// Scheduler functions
Scheduler::Scheduler() {
    // Initiate scheduler by creating schedule queue and putting
    // in initial events.
    events_p = new std::priority_queue<Event*, std::vector<Event*>, timecomp>; 
    SYSTEM_TIME = 0;
}

void Scheduler::setTime(double time) {
    SYSTEM_TIME = time;
}

Scheduler::~Scheduler() {
    delete[] events_p;
}

void Scheduler::add(Event* event_p) {
    events_p->push(event_p);
}

bool Scheduler::doNext() {
    if (this->events_p->size() != 0) {
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

void Scheduler::printAndDestroySchedule() {
    while (this->events_p->size() != 0) {
        Event* curr = this->events_p->top();
        this->events_p->pop();
        std::cout << "Time " << curr->getTime() << std::endl;
    }
}

double Scheduler::getCurrentTime() {
    return SYSTEM_TIME;
}

// Event functions
Event::Event(double in_time, void (*fp)(void*), void *arg)
    :time(in_time)
    ,arg(arg)
    ,fp(fp)
{ }

void Event::execute() {
    this->fp(this->arg);
}

const double Event::getTime() { 
    return time; 
}

double Controller::getThroughput() {
    double t = 0;
    for (std::list<Link *>::iterator it = links.begin(); it != links.end();
            it++)
    {
        t += (*it)->getDataSent();
    }
    return t;

}

double Controller::getLinkLoss(double loss) {
    for (std::list<Link *>::iterator it = links.begin(); it != links.end();
            it++)
    {
        loss += (*it)->getPacketLoss();
    }
    return loss;
}

void Controller::makeDebugPlots() {
    makePlots(&plotOptions); 
}


void Controller::deleteFlow(Packet *p) {
    Host *h = (Host *)p->getSource();
    Flow *f = h->getFlow();
    f->end();
    std::cout << "\n\n\nFlow " << f->infoString() << " can't reach its "
        << "destination.\nConsequently, it will be terminated.\n\n\n";
    sleep(5);
}

