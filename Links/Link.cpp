#include "Link.h"

extern Controller *SYSTEM_CONTROLLER; 
void sendAnotherPacket(void *arg);

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
{ 
    nextFree = SYSTEM_CONTROLLER->getCurrentTime();
    in_end1->addLink(this);
    in_end2->addLink(this);
}

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

std::string Link::infoString(){
    std::stringstream ss;
    ss << "(Link " << this->getId() << ")";
    return ss.str();
}

void Link::handlePacket(Packet* packet) {
    std::cout << "T" << SYSTEM_CONTROLLER->getCurrentTime() << \
        ":" << this->infoString() << " is handling "<< \
        packet->infoString() << std::endl;
    int size = packet->getSize();
    // If there is space remaining in the buffer
    if (!(size + this->capacityUsed > this->capacity)) {
        // Add the packet
        this->buffer.push(packet);
        this->capacityUsed += size;
        unsigned int time;
        unsigned int currentTime = SYSTEM_CONTROLLER->getCurrentTime();
        if (this->nextFree > currentTime) {
            time = nextFree + packet->getSize() / rate;
        }
        else {
            time = currentTime;
        }
        Event *e = new Event(time, &sendAnotherPacket, this);
        SYSTEM_CONTROLLER->add(e);
        nextFree = time;
    }
    else {
        // Otherwise, delete the packet
        delete packet;
        this->packetLoss += size;
    }
}

Packet* Link::popPacket() {
    assert (this->buffer.size() != 0);
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

double Link::getStat(std::string stat, int period) {
    if (stat.compare("occupancy") == 0) {
        return (double) this->getOccupancy();
    }
    else if (stat.compare("loss") == 0) {
        return (double) this->getPacketLoss() / period;
    }
    else if (stat.compare("data sent") == 0) {
        return (double) this->getDataSent() / period;
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
    n->handlePacket(p);
    // Clean up
    free(argArray);
}

void sendAnotherPacket(void *arg) {
    // Get the current time and propagation time to schedule the next event
    Link * link = (Link *)arg;
    unsigned int currentTime = SYSTEM_CONTROLLER->getCurrentTime();
    unsigned int propogationTime = link->getDelay();
    // Pop the next packet
    Packet *packet = link->popPacket();
    assert(packet != NULL);
    if (packet->getType() == Packet::ROUTERROUTE && SYSTEM_CONTROLLER->getCurrentTime() == 10) {
        int x = 1;
    }
    // Get the next node, which is the opposite end from which the
    // packet came from.
    Node *nextNode;
    if (link->getEnd2() == packet->getPreviousNode()) {
        nextNode = link->getEnd1();
    }
    else {
        nextNode = link->getEnd2();
    }
    // Store the node and the packet as an argument for the callback
    // The array `args` WILL BE FREED in the callback, so can't be
    // used afterward.
    void **args = (void **)malloc(sizeof(void *) * 3);
    args[0] = nextNode;
    args[1] = packet;
    args[2] = link;
    // Make a callback for the event to execute
    void (*fp)(void*) = &sendPacketCallback;
    // Make a new event and add it to the controller's schedule
    Event *e = new Event(currentTime + propogationTime, fp, (void *)args);
    SYSTEM_CONTROLLER->add(e);
}

int Link::getDelay() {
    return this->delay;
}

int Link::getId(){
    return this->ID;
}
