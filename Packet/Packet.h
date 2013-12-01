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

// Abstract class that holds basics of a packet
class Packet{

public:
    enum PacketType {
        ROUTERROUTE,
        HOSTROUTE,
        DATA,
        ACK
    };
        // The types of packets that may exist

    // Constants for data size
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
    virtual ~Packet();
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

// Abstract packet class used for routing, either host or router
class RoutingPacket : public Packet {
protected:
    Link* link;
        // The link the packet came on
public:
    RoutingPacket(Node *source, Node *destination, 
        Link* in_link, PacketType in_type, int in_size = 1024);
        // Create an object of class RoutingPacket with the given
        // specifications.
    ~RoutingPacket();
        // Destroy this instance of RoutingPacket

    Link* getLink();
        // Returns the link the packet came on
    virtual std::string infoString() = 0;
        // Returns a string formatted to describe the packet.
};

// The router packet type that routers will send out for routing table
// updates.
class RouterRoutingPacket : public RoutingPacket {
protected:
    RoutingTable *table; 
        // The routing table stored in the packet
public:
    RouterRoutingPacket(Node *source, Node *destination,
        Link *in_link, RoutingTable *in_table, int size = 1024);
        // Create an object of class RouterRoutingPacket with the
        // given specifications.
    ~RouterRoutingPacket();
        // Destroy this instance of RouterRoutingPacket
    RoutingTable* getRoutingTable();
        // Returns the routing table stored in the packet
    virtual std::string infoString();
        // Returns a string formatted to describe the packet
};

// The packet type that hosts will send to routers to show them
// that they exist.
class HostRoutingPacket : public RoutingPacket {
protected:
    Host *host;
        // THe host that is identifying itself
public:
    HostRoutingPacket(Node *source, Node *destination, Link *in_link,
        Host *in_host, int size = 1024);
        // Create an object of class HostRoutingPacket with the given
        // specifications.
    ~HostRoutingPacket();
        // Destroy this instance of HostRoutingPacket
    Host *getHost();
        // Returns the host stored in the packet
    virtual std::string infoString();
        // Returns a string formatted to describe the packet
};

// The packet type used to transport the data of a flow.
class DataPacket : public Packet {
protected:
    int packetId;
        // The id of the packet
    double startTime;
        // The time this packet was sent from its destination
    Flow *flow_p;
        // The flow this packet is associated with.

public:
    DataPacket(int id, Flow *flow, double in_startTime);
        // Create an object of class DataPacket with the given specifications.
    DataPacket(DataPacket *old);
        // Copy a given DataPacket
    ~DataPacket();
        // Destroy this instance of DataPacket

    int getId();
        // Returns the packet id
    double getStartTime();
        // Returns time packet originally sent. 
    Flow* getFlow();
        // Return the flow this packet is associated to.
    virtual std::string infoString();
        // Prints status information about this packet
};

// The packet type used to send back acknowledgements.
class AckPacket : public DataPacket {
private:
    int ackId;
        // The id of the packet
public:
    AckPacket(DataPacket* packet);
        // Create an object of class AckPacket with the given specifications.
    AckPacket(DataPacket* packet, int id);
        // Create an object of class AckPacket with the given specifications.
    ~AckPacket();
        // Destry this instance of AckPacket.
    int getId();
        // returns ack ID. Same as packet ID if no packets dropped.
        // otherwise, might be lower. 
    virtual std::string infoString();
        // Returns a string formatted to describe the packet
};

#endif

