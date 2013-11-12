// Flow.cpp

#include "Flow.h"

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
    this->windowSize = 10;
    this->timeout = 400;
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

void Flow::handlePacket(AckPacket *p) {
    std::cout << "Flow " << flowId << " has received an acknowledgement packet "
        "at time " << SYSTEM_CONTROLLER->getCurrentTime() << "\n";
    // remove the packet from outstanding if it exists
    std::vector<DataPacket *>::iterator it = std::find(this->outstanding.begin(), this->outstanding.end(), p);
    if (it != this->outstanding.end())
    {
        this->outstanding.erase(it);
    }
    // add it to done packets
    this->packets[p->getId()] = 1;
    // TODO: update window size
    // update other stats: progress, dataSent, dataReceived
    this->progress++;
    std::cout << "Flow " << flowId << " progress: " << progress <<
        " out of " << totalPackets << " received\n";
    this->dataReceived += p->getSize();

    // DEBUG
    if (this->progress == this->totalPackets) {
        std::cout << "Flow " << this->flowId << " has finished " <<
            "sending and receiving all packets.\n";
    }

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
    // Add the packet to the list of outstanding packets
    flow->outstanding.push_back(p);
    flow->updateDataSent(p->getSize());
}

void Flow::maintain() {
    // If this flow is done sending packets, inform the system controller and
    // stop making calls to this function
    if (this->progress == this->totalPackets) {
        SYSTEM_CONTROLLER->decrementFlowsLeft();
        return;
    }

    if (this->outstanding.size() > 0 &&
            SYSTEM_CONTROLLER->getCurrentTime() - 
            this->outstanding.front()->getStartTime() > this->timeout)
    {
        // Reset the outstanding packets because we assume they've been lost
        this->outstanding.clear();
        // The packet waiting has timed out...assume that the first
        // outstanding packet has been lost and resend it.
        makeAndSendPacket(this->outstanding.front()->getId(), this);
    }

    // If the flow/host hasn't surpassed the window size
    else if (this->outstanding.size() < this->windowSize) {
        int id = this->getNextPacketId();
        // And there is still a packet to send
        if (id != FLOW_END) {
            // Then send the packet
            makeAndSendPacket(id, this);
        }
    }

    if (this->progress < this->totalPackets) {
        // Schedule the next maintenance for this flow to be frequent
        Event *e = new Event(SYSTEM_CONTROLLER->getCurrentTime() +
            FLOW_MAINTENANCE_PERIOD, &maintainFlowCallback, this);
        SYSTEM_CONTROLLER->add(e);
    }
}

int Flow::getNextPacketId() {
    int id = FLOW_END;
    if (this->outstanding.size() != 0) {
        DataPacket *last = this->outstanding.back();
        id = last->getId() + 1;
    }
    else {
        for (int i = 0; i < totalPackets; i++) {
            if (this->packets[i] == 0) {
                id = i;
                break;
            }
        }
    }
    if (id == FLOW_END || id >= this->totalPackets) {
        return FLOW_END;
    }
    return id;
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
        return (double)this->outstanding.size() * (double)period /
            (double)this->dataReceived;
    }
}
