// Packet.h

#ifndef PACKET_H
#define PACKET_H

#include <cassert>
#include <string>

#include "RoutingTable.h"

class RoutingTable;
class Node;
class Flow;
class Link;

/* Abstract class that holds basics of a packet */
class Packet{

public:
    /* Enum for packet type */
    enum PacketType {
        ROUTE,
        DATA,
        ACK
    };

    /* Constants for data size */
    const static int DATASIZE  = 1024;
    const static int ACKSIZE   = 64;


protected:
    PacketType type;
        // Specifies the type of packet this is
    int size;
        // The size of this packet
    Node *source;
        // The source of this packet
    Node *destination;
        // The destination of this packet

public:
    Packet(PacketType in_type, int in_size, Node *s, Node *de);
        // Create an object of class Packet with the given specifications
    ~Packet();
        // Destroy this instance of Packet

    int getSize();
        // Return the size of the packet
    PacketType getType();
        // Return the type of the packet
    Node* getSource();
        // Return the source node of the packet
    Node* getDestination();
        // Return the destination node of the packet
};


class RoutingPacket : public Packet {
protected:
    RoutingTable *table; 
    Link* link;
public:
    RoutingPacket(Node *source, Node *destination, \
                  RoutingTable *in_table, Link* in_link, int size = 1024);
    ~RoutingPacket();

    RoutingTable* getRoutingTable();
    Link* getLink();
};

class DataPacket : public Packet {
protected:
    int packetId;
        // The id of the packet
    unsigned int startTime;
        // The time this packet was sent from its destination
    Flow *flow_p;
        // The flow this packet is associated with. If it is a Routing
        // Table Update packt, this should be NULL.

public:
    DataPacket(int id, Flow *flow, unsigned int in_startTime);
    DataPacket(DataPacket *old);
    ~DataPacket();

    int getId();
        // Returns the packet id
    unsigned int getStartTime();
        // Returns time packet originally sent. 
    Flow* getFlow();
        // Return the flow this packet is associated to. If the packet is
        // a Routing Table Update packet, this will be NULL.
};

class AckPacket : public DataPacket {
public:
    AckPacket(DataPacket* packet);
    ~AckPacket();

};

#endif
