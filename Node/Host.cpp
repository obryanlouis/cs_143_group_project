// Host.cpp

#include "Host.h"

Host::Host(int in_id)
    : dataSent(0)
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

void Host::setFlow(Flow *flow) {
    this->flow_p = flow;
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
    std::cout << "Host " << this->nodeId << " is handling packet\n";

    time_t time;     // to be used for handling data packets
    AckPacket *ack;     // to be used for handling data packets
    Flow *flow = this->flow_p;      // to be used for handling data packets
    HostRoutingPacket *routingPacket;

    switch (packet->getType())
    {
    case Packet::ROUTERROUTE:
        // Hosts need to send back information to the router telling
        // it that they are there.
        routingPacket = new HostRoutingPacket(NULL, NULL, 
                this->links.front(), this);
        this->links.front()->handlePacket(routingPacket);
        break;
    case Packet::ACK:
        // handle acknowledgement packets: adjust the receive rate of the
        // host, do work for congestion control (not needed just yet)
        dataReceived += packet->getSize();
        // Create a new acknowledgement packet and send it
        ack = new AckPacket((DataPacket *)packet);
        flow->handlePacket(ack);
        break;
    case Packet::DATA:
        // If the source of this packet is this host, then the packet
        // is coming from this host's flow, so it should be sent.
        // Otherwise, the host is receiving this data.
        if (packet->getSource() == this) {
            // Update the data sent
            this->dataSent += packet->getSize();
            // Send this packet
            this->links.front()->handlePacket(packet);
        }
        else {
            // handle data packets: adjust its own receive rate, but also look
            // at the packet's flow and adjust that flow's receive rate
            dataReceived += packet->getSize();

            // also needs to send back an acknowledgement packet
            // TODO: Get time from scheduler
            time = 0;
            *ack = new AckPacket((DataPacket *)packet);
            // Send the packet back to the host
            this->links.front()->handlePacket(ack);
        }
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
