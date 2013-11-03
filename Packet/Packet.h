

#ifndef PACKET_H
#define PACKET_H

#include <cassert>
#include <string>
#include "Flow.h"

class RoutingTable;

class Packet{
public:
    enum PacketType {
        INF,
        DATA,
        ACK
    };
private:
    RoutingTable *table;
    PacketType type;
    std::string id;
    Host *source;
    Host *destination;
    time_t startTime;
    int size;
    Flow *flowId;

public:
    PacketType getType();
    Packet(RoutingTable *t, PacketType typ, Host *s, Host *de, time_t start, 
            int size, Flow *f, std::string id);
    ~Packet();
    RoutingTable * getRoutingTable();
    int getSize();

};

#endif
