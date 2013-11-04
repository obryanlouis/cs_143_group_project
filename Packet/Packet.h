

#ifndef PACKET_H
#define PACKET_H

#include <cassert>
#include <string>
#include "Flow.h"

class RoutingTable;
class Node;

class Packet{

public:
    enum PacketType {
        ROUTE,
        DATA,
        ACK
    };
private:
    RoutingTable *table;
    PacketType type;
    std::string id;
    Node *source;
    Node *destination;
    time_t startTime;
    int size;
    Flow *flowId;

public:
    Packet(RoutingTable *t, PacketType typ, Node *s, Node *de, time_t start, 
            int size, Flow *f, std::string id);
    ~Packet();
    RoutingTable * getRoutingTable();
    int getSize();
    PacketType getType();
    Node* getDestination();

};

#endif
