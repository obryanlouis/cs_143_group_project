#include "Link.h"

Link::Link(int in_ID, Node *in_end1, Node *in_end2, int in_capacity,
    int in_delay)
    : ID(in_ID)
    , end1_p(in_end1)
    , end2_p(in_end2)
    , capacity(in_capacity)
    , delay(in_delay)
    , dataSent(0)
    , packetLoss(0)
    , capacityUsed(0)
{ }

void Link::resetStats() {
    this->dataSent = 0;
    this->packetLoss = 0;
}

int Link::getLength() { 
    return this->length; 
}

Node *Link::getEnd1() { 
    return this->end1_p; 
}

Node *Link::getEnd2() { 
    return this->end2_p; 
}

void Link::setEnds(Node *n1, Node *n2) {
    this->end1_p = n1;
    this->end2_p = n2;
}

void Link::handlePacket(Packet* packet) {
    int size = packet->getSize();
    // If there is space remaining in the buffer
    if (!(size + this->capacityUsed > this->capacity)) {
        // Add the packet
        this->buffer.push(packet);
        this->capacityUsed += size;
    }
    else {
        // Otherwise, delete the packet
        delete packet;
        this->packetLoss += size;
    }
}

Packet* Link::popPacket() {
    Packet *packet = this->buffer.back();
    this->buffer.pop();
    this->dataSent += packet->getSize();
    this->capacityUsed -= packet->getSize();
    return packet;
}

int Link::getOccupancy() {
    return this->capacityUsed;
}

int Link::getPacketLoss() {
    return this->packetLoss;
}

int Link::getFlowRate() {
    return this->dataSent;
}

int Link::getStat(std::string stat) {
    if (stat.compare("occupancy") == 0) {
        return this->getOccupancy();
    }
    else if (stat.compare("loss") == 0) {
        return this->getPacketLoss();
    }
    else if (stat.compare("rate") == 0) {
        return this->getFlowRate();
    }
    throw new std::string("You tried to compute the stat " + stat
        + " but this stat doesn't exist.");
}
