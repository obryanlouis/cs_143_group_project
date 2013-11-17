// Flow.cpp

#include "Flow.h"
#include <sstream>
#include <climits>

extern Controller *SYSTEM_CONTROLLER; 

Flow::Flow(int in_ID, int in_size, Host *in_source, Host *in_destination)
    : flowId(in_ID)
    , size(in_size)
    , source(in_source)
    , destination(in_destination)
    , progress(0)
    , dataSent(0)
    , dataReceived(0)
{
    // Calculate the number of packets we need based on size of flow 
    this->totalPackets = (in_size + (Packet::DATASIZE - 1)) / Packet::DATASIZE;
    for (int i = 0; i < totalPackets; i++){
        packets[i] = 0;
    }
    this->windowSize = 5;
    this->timeout = 4000;
    in_source->setFlow(this);
}

Flow::~Flow()
{}

void Flow::resetStats() {
    dataSent = 0;
    dataReceived = 0;
}

void Flow::updateDataSent(int bytes) {
    dataSent += bytes;
}

void Flow::updateDataReceived(int bytes) {
    dataReceived += bytes;
}

std::string Flow::infoString(){
    std::stringstream ss;
    ss << "(Flow " << this->getId() << ")";
    return ss.str();
}

void Flow::handlePacket(AckPacket *p) {
    std::cout << "Flow " << flowId << " has received an acknowledgement packet "
        "at time " << SYSTEM_CONTROLLER->getCurrentTime() << "\n";

    // update progress
    if (this->packets.count(p->getId()) != UINT_MAX){
        this->progress++;
    }  
    // done with that packet ID, so set its timeout to super long
    // so it'll never get sent
    
    this->packets[p->getId()] = UINT_MAX; 
    

    // TODO: update window size
    this->windowSize += 1;

    // update other stats: dataSent, dataReceived
    std::cout << "Flow " << flowId << " progress: " << progress <<
        " out of " << totalPackets << " received\n";
    this->dataReceived += p->getSize();

    delete p;
}

void maintainFlowCallback(void *arg) {
    Flow *flow = (Flow *)arg;
    flow->maintain();
}

void makeAndSendPacket(int id, Flow *flow) {
    DataPacket *p = new DataPacket(id, flow, SYSTEM_CONTROLLER->getCurrentTime());
    // Send the packet
    std::cout << "Flow " << flow->getId() << " is sending packet " 
        << id << " to Host " << flow->source->getId() << "\n";
    flow->source->handlePacket(p);
}

void Flow::maintain() {
    // If this flow is done sending packets, inform the system controller and
    // stop making calls to this function by not making new maintain event
    if (this->progress == this->totalPackets) {
        SYSTEM_CONTROLLER->decrementFlowsLeft();
        return;
    }

    // check if there are any dropped packets. If so, update window size.
    bool dropped = false;
    for (int i = 0; i < totalPackets; i++){
        if (packets[i] < SYSTEM_CONTROLLER->getCurrentTime() && packets[i] != 0) dropped = true;
    }
    if (dropped) this->windowSize /= 2;
    if (this->windowSize == 0) this->windowSize = 1;

for (int i = 0; i < totalPackets; i++){
    std::cout << i << "has timeout of " << packets[i] << std::endl;}


    // make and send packet 
    int packetsSent = 0;
    for (int i = 0; i < totalPackets; i++){
        if (this->packets.count(i) > 0
            && packets[i] <= SYSTEM_CONTROLLER->getCurrentTime()
            && packetsSent < windowSize) {
            
            makeAndSendPacket(i, this);
            packets[i] = SYSTEM_CONTROLLER->getCurrentTime() + this->timeout;
            ++packetsSent;
        }
    }



    std::cout << "sent out more packets" << std::endl;
    // update number of outstanding packets
    packetsSent = 0;
    for (int i = 0; i < totalPackets; i++){
        if (packets[i] > 0){
            packetsSent++;
        }
    }
    this->outstanding = packetsSent;

    if (this->progress < this->totalPackets) {
        // Schedule the next maintenance for this flow to be frequent
        Event *e = new Event(SYSTEM_CONTROLLER->getCurrentTime() +
            FLOW_MAINTENANCE_PERIOD, &maintainFlowCallback, this);
        SYSTEM_CONTROLLER->add(e);
    }
}

            
Host *Flow::getStart(){
    return source;
}

Host *Flow::getDestination(){
    return destination;
}

int Flow::getId() {
    return this->flowId;
}

double Flow::getStats(std::string stat, int period) {
    // TODO: Make sure all the units are correct.
    if (stat.compare("send rate") == 0) {
        return (double)this->dataSent / (double)period;
    }
    else if (stat.compare("receive rate") == 0) {
        return (double)this->dataReceived / (double)period;
    }
    else if (stat.compare("rtt") == 0) {
        return (double)this->outstanding * (double)period /
            (double)this->dataReceived;
    }
}
