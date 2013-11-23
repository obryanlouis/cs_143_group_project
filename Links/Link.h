

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

/* Link class which controls packet transfer between routers. */
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
        // units: ms
    Buffer *buffer1;
    Buffer *buffer2;
        // FIFO queue of packets
    double dataSent;
        // used for stat keeping
    double packetLoss;
        // used for stat keeping 
	double rate;	
	    // the link rate in bytes / sec
    int pushPacket(Packet *in_packet);
        // push the packet onto the buffer. Returns 0 if successfuly and
        // 1 if the packet is dropped
    double nextFree;
        // The next time that the queue will be free (ms)
    Buffer * getBuffer(Node *end);


public:
    Link () { }
    Link(int in_ID, Node *in_end1, Node *in_end2, double in_capacity,
        double in_delay, double in_rate);

    ~Link() { }
    
	void resetStats();
	double getOccupancy();
	double getPacketLoss();
	double getDataSent();
	double getStat(std::string stat, double period);
	double getRate();
	double getDelay();
	
    int getId();
	Node *getEnd1();
	Node *getEnd2();
    void setEnds(Node *n1, Node *n2);
    std::string infoString();

    Packet* popPacket(Node *end);
    void handlePacket(Packet* packet);
        // Handles an incoming packet by either adding it to the buffer, or 
        // dropping it. Also schedules an event to execute that will simulate
        // the router processing THIS packet.

};

#endif
