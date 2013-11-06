#include "Packet.h"

/***************** Packet Functions ***************************/
Packet::Packet(PacketType in_type, int in_size, Node *s, Node *de)
    : type(in_type)
    , source(s)
    , destination(de)
    , size(in_size)
{}

Packet::~Packet() { } 

int Packet::getSize() {
    return this->size;
}

Packet::PacketType Packet::getType() {
    return this->type;
}

Node* Packet::getSource() {
    return this->source;
}

Node* Packet::getDestination() {
    return this->destination;
}


/**************** RoutingPacket ********************/
RoutingPacket::RoutingPacket(Node *source, Node *destination, \
                        RoutingTable *in_table, Link* in_link, int in_size)
    :Packet(Packet::ROUTE, in_size, source, destination)
    ,link(in_link) 
{   
    this->table = new RoutingTable(in_table);
}

RoutingPacket::~RoutingPacket(){
    delete &table;
}

RoutingTable* RoutingPacket::getRoutingTable(){
    return this->table;
}

Link* RoutingPacket::getLink(){
    return this->link;
}


/************* DataPacket ***********************/
DataPacket::DataPacket(int in_id, Flow *flow, unsigned int in_startTime)
    :Packet(Packet::DATA, Packet::DATASIZE,\
           (Node *)(flow->getStart()), (Node *)(flow->getDestination()))
    , packetId(in_id)
    , flow_p(flow)
    , startTime(in_startTime)
{
}

DataPacket::DataPacket(DataPacket *old)
    :Packet(old->getType(), old->getSize(), \
            old->getSource(), old->getDestination())
    , packetId(old->packetId)
    , flow_p(old->flow_p)
    , startTime(old->startTime)
{    
}


DataPacket::~DataPacket() {}

int DataPacket::getId(){
    return this->packetId;
}

unsigned int DataPacket::getStartTime(){
    return this->startTime;
}

Flow* DataPacket::getFlow() {
    return this->flow_p;
}

/**** AckPacket *****/
AckPacket::AckPacket(DataPacket *old)
    :DataPacket(old)
{
    type = Packet::ACK;
    size = Packet::ACKSIZE;
}

AckPacket::~AckPacket(){}
