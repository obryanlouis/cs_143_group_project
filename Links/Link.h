

#ifndef LINK_H
#define LINK_H

#include <utility>
#include <queue>
#include <string>
#include "Main.h"
#include "Node.h"
#include "Control.h"

class Node;
class Packet;
class Controller;

namespace {
    extern Controller *CONTROLLER;
}

/* Link class which controls packet transfer between routers. */
class Link {
	
private:
    int ID;
        // ID of the link
	Node *end1_p;
        // One endpoint of the link
	Node *end2_p;
        // The other endpoint of the link
    int capacity; 
        // maximum capacity (in bytes)
    int capacityUsed;
        // the current amount of the buffer's capacity that has been used
    int delay; 
        // time delay for travel through link (ignoring in/out time)
    std::queue<Packet*> buffer; 
        // FIFO queue of packets
    int dataSent;
        // used for stat keeping
    int packetLoss;
        // used for stat keeping 
	int rate;	
	    // the link rate in Mbps
    int pushPacket(Packet *in_packet);
        // push the packet onto the buffer. Returns 0 if successfuly and
        // 1 if the packet is dropped


public:
    Link () { }
    Link(int in_ID, Node *in_end1, Node *in_end2, int in_capacity,
        int in_delay, int in_rate);

    ~Link() { }
    
	void resetStats();
	int getOccupancy();
	int getPacketLoss();
	int getDataSent();
	int getStat(std::string stat);
	int getRate();
	int getDelay();
	
	Node *getEnd1();
	Node *getEnd2();
    void setEnds(Node *n1, Node *n2);

    Packet* popPacket();
    void handlePacket(Packet* packet);
        // Handles an incoming packet by either adding it to the buffer, or 
        // dropping it. Also schedules an event to execute that will simulate
        // the router processing THIS packet.
    void sendAnotherPacket();
        // Dequeues the top packet and schedules an event in the future for
        // the router to handle the packet.

};

#endif
