// Host.cpp

#include "Host.h"

Host::Host(int in_id, Link *in_link, Flow *in_flow)
    : flow_p(in_flow)
    , dataSent(0)
    , dataReceived(0)
{
    std::list<Link*> l;
    l.push_back(in_link);

    Node(in_id, l);
}

Host::~Host() {
    delete this->flow_p;
}

Link* Host::getLink() {
    if (Node::l.size() != 1) {
        std::cout << "Error: Host does not have exactly one link." << std::endl;
        assert(0);
    }

    return (*list.begin());
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
    Packet *ack;     // to be used for handling data packets
    Flow *flow;      // to be used for handling data packets

    switch (packet->getType())
    {
    case Packet::ROUTE:
        // Hosts don't need to do anything with routing table update packets
        break;
    case Packet::ACK:
        // handle acknowledgement packets: adjust the receive rate of the
        // host, do work for congestion control (not needed just yet)
        dataReceived += packet->getSize();
        break;
    case Packet::DATA:
        // handle data packets: adjust its own receive rate, but also look
        // at the packet's flow and adjust that flow's receive rate
        dataReceived += packet->getSize();

        // Update stats of the flow
        flow = packet->getFlow();
        flow->updateDataReceived(packet->getSize());

        // also needs to send back an acknowledgement packet
        // TODO: Get time from scheduler
        time = 0;
        Packet *ack = new Packet(0, Packet::ACK, packet->getDestination(),
                packet->getSource(), time /*fix*/, Packet::ACKSIZE,
                flow, packet->getId());
        // TODO: send the packet out back to the original host.
        break;
    default:
        std::cout << "INVALID PACKET TYPE" << std::endl;
        assert(0);
        break;
    }
}

