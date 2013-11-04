#include "Packet.h"

using namespace std;

Packet::~Packet() { } 

Packet::Packet(RoutingTable *tab, Packet::PacketType t, Host *s, Host *de, time_t start,
        int size, Flow *f, string id) {
    this->table = tab;
    this->type = t;
    this->source = s;
    this->destination = de;
    this->startTime = start;
    this->size = size;
    this->flowId = f;
    this->id = id;
}

Packet::PacketType Packet::getType() {
    return type;
}

RoutingTable * Packet::getRoutingTable() {
    assert(this->type == Packet::ROUTE);
    return this->table;
}

int Packet::getSize() {
    return this->size;
}
