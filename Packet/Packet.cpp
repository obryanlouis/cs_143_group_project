#include "Packet.h"


Packet::Packet(RoutingTable *tabl, Packet::PacketType type, Node *s, Node *de,
        time_t start, Packet::PacketSize packetSize, Flow *f, std::string id)
    : table(table)
    , type(type)
    , source(s)
    , destination(de)
    , startTime(start)
    , size(packetSize)
    , flow_p(f)
    , packetId(id)
{}

Packet::~Packet() { } 

RoutingTable* Packet::getRoutingTable() {
    assert(this->type == Packet::ROUTE);
    return this->table;
}

Packet::PacketSize Packet::getSize() {
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

Flow* Packet::getFlow() {
    assert(this->type != Packet::ROUTE);
    return this->flow_p;
}
