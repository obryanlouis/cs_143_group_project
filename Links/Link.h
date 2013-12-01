// Link.h

#ifndef LINK_H
#define LINK_H

#include <utility>
#include <queue>
#include <string>
#include <sstream>
#include "Node.h"
#include "Control.h"
#include "Buffer.h"

class Node;
class Packet;
class Buffer;

// Link class which controls packet transfer between routers.
class Link {
	
private:
    int ID;
        // ID of the link
	Node *end1_p;
        // One endpoint of the link
	Node *end2_p;
        // The other endpoint of the link
    double capacity; 
        // maximum capacity (in bytes)
    double delay; 
        // time delay for travel through link (ignoring in/out time)
        // in ms
    Buffer *buffer1;
    Buffer *buffer2;
        // FIFO queue of packets from both ends of the link
    double dataSent;
        // amount of data sent in the last snapshot time in bytes
    double packetLoss;
        // amount of data lost in the last snapshot time in bytes
	double rate;	
	    // the link rate in bytes / sec
    int pushPacket(Packet *in_packet);
        // push the packet onto the buffer. Returns 0 if successfuly and
        // 1 if the packet is dropped
    double nextFree;
        // The next time that the queue will be free (ms)

    Buffer * getBuffer(Node *end);
        // Gets the buffer of the link that goes to the given node. Fails
        // if the node is not one of the endpoints.

public:
    Link () { }
        // Create an object of type Link.
    Link(int in_ID, Node *in_end1, Node *in_end2, double in_capacity,
         double in_delay, double in_rate);
        // Create an object of type Link with teh given specifications.

    ~Link() { }
        // Destroy this instance of Link.
    
	void resetStats();
        // Reset the stats on the link (to be used every snapshot time)
	double getOccupancy();
        // Returns the occupancy of the link's buffers
	double getPacketLoss();
        // Returns the packet loss of the link in the last snapshot time
        // in bytes
	double getDataSent();
        // Returns the amount of data sent over the link in the last
        // snapshot time in bytes
	double getStat(std::string stat, double period);
        // Returns the stat requested by the stat string.
        // Options are: "occupancy", "loss", "data sent".
	double getRate();
        // Returns the link rate in bytes / sec
	double getDelay();
        // Returns the link delay in ms
	
    int getId();
        // Returns the ID of the link
	Node *getEnd1();
        // Returns the first end of the link
	Node *getEnd2();
        // Returns the second end of the link
    void setEnds(Node *n1, Node *n2);
        // Sets the ends of the link
    std::string infoString();
        // Returns a string formatted as the following: "(Link x)",
        // where x is the link id.

    Packet* popPacket(Node *end);
        // Pops the packet from the front of the appropriate buffer
        // and returns it.
    void handlePacket(Packet* packet);
        // Handles an incoming packet by either adding it to the buffer, or 
        // dropping it. Also schedules an event to execute that will simulate
        // the router processing THIS packet.

};

#endif

