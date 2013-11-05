#include "Link.h"

Link::Link(int in_ID, Node *in_end1, Node *in_end2, int in_capacity,
    int in_delay, int in_rate)
    : ID(in_ID)
    , end1_p(in_end1)
    , end2_p(in_end2)
    , capacity(in_capacity)
    , delay(in_delay)
    , dataSent(0)
    , packetLoss(0)
    , capacityUsed(0)
    , rate(in_rate)
{ }

void Link::resetStats() {
    this->dataSent = 0;
    this->packetLoss = 0;
}

int Link::getRate() { 
    return this->rate; 
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

int Link::getDataSent() {
    return this->dataSent;
}

int Link::getStat(std::string stat) {
    if (stat.compare("occupancy") == 0) {
        return this->getOccupancy();
    }
    else if (stat.compare("loss") == 0) {
        return this->getPacketLoss();
    }
    else if (stat.compare("data sent") == 0) {
        return this->getDataSent();
    }
    throw new std::string("You tried to compute the stat " + stat
        + " but this stat doesn't exist.");
}

void sendPacketCallback(void* args) {
    // Unpack the arguments
    void **argArray = (void **)args;
    Node *n = (Node *)argArray[0];
    Packet *p = (Packet *)argArray[1];
    Link *l = (Link *)argArray[2];
    n->handlePacket(p, l);
    // Clean up
    delete argArray;
}

void Link::sendAnotherPacket() {
    // Get the current time and propagation time to schedule the next event
    unsigned int currentTime = CONTROLLER->getCurrentTime();
    unsigned int propogationTime = delay;
    // Get the next node
    Node *nextNode = this->end2_p;
    // Pop the next packet
    Packet *packet = this->popPacket();
    // A pointer to this link;
    Link *link = this;
    // Store the node and the packet as an argument for the callback
    void **args = (void **)malloc(sizeof(void *) * 3);
    args[0] = nextNode;
    args[1] = packet;
    args[2] = link;
    // Make a callback for the event to execute
    void (*fp)(void*) = &sendPacketCallback;
    // Make a new event and add it to the controller's schedule
    Event *e = new Event(currentTime + propogationTime, fp, &args);
    CONTROLLER->add(e);
}

int Link::getDelay() {
    return this->delay;
}

