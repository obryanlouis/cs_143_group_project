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
class Host;

/* Abstract class that holds basics of a packet */
class Packet{

public:
    /* Enum for packet type */
    enum PacketType {
        ROUTERROUTE,
        HOSTROUTE,
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
    Node *previousNode;
        // The previous node that this packet came from on this
        // packet's route

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
    void setPreviousNode(Node *node);
        // Set the previous node that this packet came from
    Node* getPreviousNode();
        // Return the previous node that this packet came from
    virtual std::string infoString() = 0;
        // Prints an appropriate message

};


class RoutingPacket : public Packet {
protected:
    Link* link;
public:
    RoutingPacket(Node *source, Node *destination, 
        Link* in_link, PacketType in_type, int in_size = 1024);
    ~RoutingPacket();

    Link* getLink();
    virtual std::string infoString();
};

// The packet type that routers will send out
class RouterRoutingPacket : public RoutingPacket {
protected:
    RoutingTable *table; 
public:
    RouterRoutingPacket(Node *source, Node *destination,
        Link *in_link, RoutingTable *in_table, int size = 1024);
    ~RouterRoutingPacket();
    RoutingTable* getRoutingTable();
    virtual std::string infoString();
};

// The packet type that hosts will send to routers to show them
// that they exist.
class HostRoutingPacket : public RoutingPacket {
protected:
    Host *host;
public:
    HostRoutingPacket(Node *source, Node *destination, Link *in_link,
            Host *in_host, int size = 1024);
    ~HostRoutingPacket();
    Host *getHost();
    virtual std::string infoString();

};

class DataPacket : public Packet {
protected:
    int packetId;
        // The id of the packet
    unsigned int startTime;
        // The time this packet was sent from its destination
    Flow *flow_p;
        // The flow this packet is associated with.

public:
    DataPacket(int id, Flow *flow, unsigned int in_startTime);
    DataPacket(DataPacket *old);
    ~DataPacket();

    int getId();
        // Returns the packet id
    unsigned int getStartTime();
        // Returns time packet originally sent. 
    Flow* getFlow();
        // Return the flow this packet is associated to.
    virtual std::string infoString();
        // Prints status information about this packet

};

class AckPacket : public DataPacket {
public:
    AckPacket(DataPacket* packet);
    ~AckPacket();
    virtual std::string infoString();
};

#endif
