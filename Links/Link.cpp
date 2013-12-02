// Link.cpp

#include "Link.h"

extern Controller *SYSTEM_CONTROLLER; 
void sendAnotherPacket(void *arg);

void checkMalloc(void *arg) {
    if (arg == NULL) {
        std::cout << "Out of memory.\n";
        exit(1);
    }
}

Link::Link(int in_ID, Node *in_end1, Node *in_end2, double in_capacity,
    double in_delay, double in_rate)
    : ID(in_ID)
    , end1_p(in_end1)
    , end2_p(in_end2)
    , capacity(in_capacity)
    , delay(in_delay)
    , dataSent(0)
    , packetLoss(0)
    , rate(in_rate)
    , instantaneousOccupancy(0)
{ 
    nextFree = SYSTEM_CONTROLLER->getCurrentTime();
    in_end1->addLink(this);
    in_end2->addLink(this);
    buffer1 = new Buffer(1);
    buffer2 = new Buffer(2);
}

void Link::resetStats() {
    this->dataSent = 0;
    this->packetLoss = 0;
}

double Link::getRate() { 
    return this->rate; 
}

Node *Link::getEnd1() { 
    return this->end1_p; 
}

Node *Link::getEnd2() { 
    return this->end2_p; 
}

Buffer * Link::getBuffer(Node *end) {
    assert(end1_p == end || end2_p == end);
    if (end1_p == end)
        return buffer1;
    return buffer2;
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
    /*std::cout << "T " << SYSTEM_CONTROLLER->getCurrentTime() << \
        ":" << this->infoString() << " is handling "<< \
        packet->infoString() << std::endl;*/
    int type = packet->getType();
    assert(type >= 0 && type <= 3);
    double size = packet->getSize();
    Node *prev = packet->getPreviousNode();

    // DEBUG
    SYSTEM_CONTROLLER->checkPackets();

    Buffer *buffer = getBuffer(prev);
    // If there is space remaining in the buffer
    if (!(size + buffer->capacityUsed > this->capacity)) {
        // Add the packet
        buffer->push(packet);
        /*std::cout << "\t\t\t\t\t\t"
                  << this->infoString() 
                  << " added packet at " << packet
                  << " to buffer "
                  << buffer->infoString()
                  << "\n";*/
        buffer->capacityUsed += size;
        /*std::cout << "Buffer capacity used: " << buffer->capacityUsed
            << " out of " << this->capacity << "\n";*/
        double time;
        double currentTime = SYSTEM_CONTROLLER->getCurrentTime();
        // Processing time in milliseconds
        double processingTime = (double)1000 * 
                (double)packet->getSize() / (double)rate;
        if (buffer->nextFree > currentTime) {
            time = buffer->nextFree + processingTime;
        }
        else {
            time = currentTime + processingTime;
        }
        void ** args = (void **)malloc(2 * sizeof(void *));
        checkMalloc(args);
        args[0] = this;
        args[1] = prev;
        Event *e = new Event(time, &sendAnotherPacket, (void *)args);
        SYSTEM_CONTROLLER->add(e);
        buffer->nextFree = time;
    }
    else {
        // Otherwise, delete the packet
        /*std::cout << "Link " << infoString() << " deleted packet "
            << packet->infoString() << "\n";*/
        delete packet;
        this->packetLoss += size;
    }
}

Packet* Link::popPacket(Node *end) {
    Buffer *buffer = getBuffer(end);
    Packet *packet = buffer->front();
    buffer->pop();
    this->dataSent += packet->getSize();
    buffer->capacityUsed -= packet->getSize();
    return packet;
}

void Link::setInstantaneousOccupancy() {
    this->instantaneousOccupancy = this->getOccupancy();
}

double Link::getInstantaneousOccupancy() {
    return this->instantaneousOccupancy;
}

double Link::getOccupancy() {
    return buffer1->capacityUsed + buffer2->capacityUsed;
}

double Link::getPacketLoss() {
    return this->packetLoss;
}

double Link::getDataSent() {
    return this->dataSent;
}

double Link::getStat(std::string stat, double period) {
    if (stat.compare("occupancy") == 0) {
        // Output is in bytes
        return (double) this->getOccupancy();
    }
    else if (stat.compare("loss") == 0) {
        // Output is in Mbps
        return (((double) this->getPacketLoss()) / period) *
            ((double) 8/ (double) 1000);
    }
    else if (stat.compare("data sent") == 0) {
        // Output is in Mbps
        return (((double) this->getDataSent()) / period) *
            ((double) 8/ (double) 1000);
    }
    throw new std::string("You tried to compute the stat " + stat
        + " but this stat doesn't exist.");
}

void sendPacketCallback(void* args) {
    // Unpack the arguments
    void **argArray = (void **)args;
    Node *n = (Node *)argArray[0];
    Packet *p = (Packet *)argArray[1];
    n->handlePacket(p);
    // Clean up
    free(argArray);
    SYSTEM_CONTROLLER->checkPackets();
}

void sendAnotherPacket(void *arg) {
    // Get the current time and propagation time to schedule the next event
    void ** theseArgs = (void **) arg;
    Link * link = (Link *)theseArgs[0];
    Node * end = (Node *)theseArgs[1];
    free(theseArgs);
    double currentTime = SYSTEM_CONTROLLER->getCurrentTime();
    double propogationTime = link->getDelay();
    // Pop the next packet
    Packet *packet = link->popPacket(end);
    
    /*std::cout << "sendAnotherPacket -- start" << "\n";
    std::cout << "\t\t\t\t\t\t"
        << "Address of packet sent by " << link->infoString() 
        << ": " << packet << "is " << packet->infoString() << "\n";*/

    // Get the next node, which is the opposite end from which the
    // packet came from.
    Node *nextNode;
    if (link->getEnd2() == packet->getPreviousNode()) {
        /*std::cout << link->infoString() << " sending packet from "
        << link->getEnd2()->infoString() << " to "
        << link->getEnd1()->infoString() << std::endl;*/
        nextNode = link->getEnd1();
    }
    else {
        /*std::cout << link->infoString() << " sending packet from "
        << link->getEnd1()->infoString() << " to "
        << link->getEnd2()->infoString() << std::endl;*/
        nextNode = link->getEnd2() ;
    }
    // Store the node and the packet as an argument for the callback
    // The array `args` WILL BE FREED in the callback, so can't be
    // used afterward.
    void **args = (void **)malloc(sizeof(void *) * 2);
    checkMalloc(args);
    args[0] = nextNode;
    args[1] = packet;
    // Make a callback for the event to execute
    void (*fp)(void*) = &sendPacketCallback;
    // Make a new event and add it to the controller's schedule
    Event *e = new Event(currentTime + propogationTime, fp, (void *)args);
    SYSTEM_CONTROLLER->add(e);

    //std::cout << "sendAnotherPacket end" << std::endl;
}

double Link::getDelay() {
    return this->delay;
}

int Link::getId(){
    return this->ID;
}

