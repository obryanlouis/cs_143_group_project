// Packet.h

#ifndef PACKET_H
#define PACKET_H

#include <cassert>
#include <string>

class RoutingTable;
class Node;
class Flow;

class Packet{

public:
    enum PacketType {
        ROUTE,
        DATA,
        ACK
    };
        // enum holding the different packet types to handle

    enum PacketSize {
        ROUTESIZE = 1024,   // not sure yet
        DATASIZE  = 1024,
        ACKSIZE   = 64
    };
        // enum holding the different packet sizes in bytes

private:
    RoutingTable *table;
        // If the packet is a Routing Table Update packet, this will be
        // a pointer to the routing table. If the packet is a data or
        // acknowledgement packet, this should be made NULL.
    PacketType type;
        // Specifies the type of packet this is
    std::string packetId;
        // The id of the packet
    Node *source;
        // The source of this packet
    Node *destination;
        // The destination of this packet
    time_t startTime;
        // The time this packet was sent from its destination
    PacketSize size;
        // The size of this packet
    Flow *flow_p;
        // The flow this packet is associated with. If it is a Routing
        // Table Update packt, this should be NULL.

public:
    Packet(RoutingTable *table, PacketType type, Node *s, Node *de, time_t start, 
            PacketSize packetSize, Flow *f, std::string id);
        // Create an object of class Packet with the given specifications
    ~Packet();
        // Destroy this instance of Packet
    std::string getId();
        // Returns the packet id
    RoutingTable* getRoutingTable();
        // Return the routing table field of this packet. If it is not
        // a Routing Table Update packet, it will just be NULL.

    PacketSize getSize();
        // Return the size of the packet
    PacketType getType();
        // Return the type of the packet
    Node* getSource();
        // Return the source node of the packet
    Node* getDestination();
        // Return the destination node of the packet
    Flow* getFlow();
        // Return the flow this packet is associated to. If the packet is
        // a Routing Table Update packet, this will be NULL.

};

#endif
