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

void Packet::setPreviousNode(Node *node) {
    this->previousNode = node;
}

Node * Packet::getPreviousNode() {
    return this->previousNode;
}

/**************** RoutingPacket ********************/
RoutingPacket::RoutingPacket(Node *source, Node *destination,\
    Link* in_link, PacketType in_type, int in_size)
    :Packet(in_type, in_size, source, destination)
    ,link(in_link) 
{ }   

RoutingPacket::~RoutingPacket() { }

Link* RoutingPacket::getLink(){
    return this->link;
}


/************** RouterRoutingPacket ******************/

RouterRoutingPacket::RouterRoutingPacket(Node *source, Node *destination,
    Link *in_link, RoutingTable *in_table, int size)
    :RoutingPacket(source, destination, in_link, 
            Packet::ROUTERROUTE, size)
{
    // Copy the routing table to this packet
    this->table = new RoutingTable(in_table);
}

RoutingTable* RouterRoutingPacket::getRoutingTable() {
    return this->table;
}

/************** HostRoutingPacket ********************/

HostRoutingPacket::HostRoutingPacket(Node *source, Node *destination,
        Link *in_link, Host *in_host, int size)
    :RoutingPacket(source, destination, in_link, 
            Packet::HOSTROUTE, size)
{
    this->host = in_host;
}

Host * HostRoutingPacket::getHost() {
    return this->host;
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
    source = old->getDestination();
    destination = old->getSource();
}

AckPacket::~AckPacket(){}
