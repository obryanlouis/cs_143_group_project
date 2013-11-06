// Host.cpp

#include "Host.h"

Host::Host(int in_id, Flow *in_flow)
    : flow_p(in_flow)
    , dataSent(0)
    , dataReceived(0)
    , Node(in_id)
{ }

Host::~Host() {
    delete this->flow_p;
}

Link* Host::getLink() {
    if (this->links.size() != 1) {
        std::cout << "Error: Host does not have exactly one link." << std::endl;
        assert(0);
    }

    return *this->links.begin();
}

Flow* Host::getFlow() {
    return flow_p;
}

int Host::getDataSent() {
    return dataSent;
}

int Host::getDataReceived() {
    return dataReceived;
}

void Host::resetStats() {
    dataSent = 0;
    dataReceived = 0;
}

void Host::handlePacket(Packet *packet){
    std::cout << "Host is handling packet" << std::endl;

    time_t time;     // to be used for handling data packets
    AckPacket *ack;     // to be used for handling data packets
    Flow *flow = this->flow_p;      // to be used for handling data packets

    switch (packet->getType())
    {
    case Packet::ROUTE:
        // Hosts don't need to do anything with routing table update packets
        break;
    case Packet::ACK:
        // handle acknowledgement packets: adjust the receive rate of the
        // host, do work for congestion control (not needed just yet)
        dataReceived += packet->getSize();
        flow->handlePacket((AckPacket *)packet);
        break;
    case Packet::DATA:
        // handle data packets: adjust its own receive rate, but also look
        // at the packet's flow and adjust that flow's receive rate
        dataReceived += packet->getSize();

        // also needs to send back an acknowledgement packet
        // TODO: Get time from scheduler
        time = 0;
        *ack = new AckPacket((DataPacket *)packet);
        // Send the packet back to the host
        this->links.front()->handlePacket(ack);
        break;
    default:
        std::cout << "INVALID PACKET TYPE" << std::endl;
        assert(0);
        break;
    }
}

void Host::setLink(Link *link) {
    this->links.clear();
    this->links.push_back(link);
}
