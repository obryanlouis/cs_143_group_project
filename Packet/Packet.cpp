#include "Packet.h"
#include <iostream>
#include <sstream>
/***************** Packet Functions ***************************/
Packet::Packet(PacketType in_type, int in_size, Node *s, Node *de)
    : type(in_type)
    , source(s)
    , destination(de)
    , size(in_size)
{
    SYSTEM_CONTROLLER->addPacket(this);
}

Packet::~Packet() { 
    std::cout << "~Packet()" << std::endl;
    SYSTEM_CONTROLLER->removePacket(this);
} 

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

RoutingPacket::~RoutingPacket() { std::cout << "    ~RoutingPacket"; }

Link* RoutingPacket::getLink(){
    return this->link;
}

//std::string RoutingPacket::infoString(){
//    std::stringstream ss;
//    std::cout << "(route pkt from" << getSource()->infoString()  << ")";
//    return ss.str(); 
//}

/************** RouterRoutingPacket ******************/

RouterRoutingPacket::RouterRoutingPacket(Node *source, Node *destination,
    Link *in_link, RoutingTable *in_table, int size)
    :RoutingPacket(source, destination, in_link, 
            Packet::ROUTERROUTE, size)
{
    // Copy the routing table to this packet
    this->table = new RoutingTable(in_table);
}

RouterRoutingPacket::~RouterRoutingPacket(){
    delete this->table;
    std::cout << "    ~RouterRoutingPacket";
}

RoutingTable* RouterRoutingPacket::getRoutingTable() {
    return this->table;
}

std::string RouterRoutingPacket::infoString(){
    std::stringstream ss;
    ss << "(route pkt from " << getSource()->infoString() << ")";
    return ss.str();
}

/************** HostRoutingPacket ********************/

HostRoutingPacket::HostRoutingPacket(Node *source, Node *destination,
        Link *in_link, Host *in_host, int size)
    :RoutingPacket(source, destination, in_link, 
            Packet::HOSTROUTE, size)
{
    this->host = in_host;
}

HostRoutingPacket::~HostRoutingPacket(){
    std::cout << "  ~HostRoutingPacket()";
}

Host * HostRoutingPacket::getHost() {
    return this->host;
}

std::string HostRoutingPacket::infoString(){
    std::stringstream ss;
    ss << "(route pkt from "<< getSource()->infoString() << ")" ;
    return ss.str();
}


/************* DataPacket ***********************/
DataPacket::DataPacket(int in_id, Flow *flow, double in_startTime)
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


DataPacket::~DataPacket() {std::cout << "   ~DataPacket";}

int DataPacket::getId(){
    return this->packetId;
}

double DataPacket::getStartTime(){
    return this->startTime;
}

Flow* DataPacket::getFlow() {
    return this->flow_p;
}

std::string DataPacket::infoString(){
    std::stringstream ss;
    ss <<"(DataPacket " << getId() << " from flow " \
            << getFlow()->getId() << ")" ;
    return ss.str();
}


/**** AckPacket *****/
AckPacket::AckPacket(DataPacket *old)
    :DataPacket(old)
{
    type = Packet::ACK;
    size = Packet::ACKSIZE;
    source = old->getDestination();
    destination = old->getSource();
    delete old;
}

AckPacket::~AckPacket(){std::cout << "    ~AckPacket";}

std::string AckPacket::infoString(){
    std::stringstream ss;
    ss << "(AckPacket " << getId() << "from flow " << \
        getFlow()->getId() << ")";
    return ss.str();
}
