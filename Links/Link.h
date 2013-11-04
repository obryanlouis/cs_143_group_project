

#ifndef LINK_H
#define LINK_H

#include <queue>
#include <string>
#include "Node.h"

class Node;
class Packet;


/* Link class which controls packet transfer between routers. */
class Link {
	
private:
    int ID;
	Node *end1_p;
	Node *end2_p;
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
	int length;	
	    // can change this
    void pushPacket(Packet *in_packet);


public:
    Link () { }
    Link(int in_ID, Node *in_end1, Node *in_end2, int in_capacity,
        int in_delay);

    ~Link() { }
    
	void resetStats();
	int getOccupancy();
	int getPacketLoss();
	int getFlowRate();
	int getStat(std::string stat);
	int getLength();
	
	Node *getEnd1();
	Node *getEnd2();
    void setEnds(Node *n1, Node *n2);

    Packet* popPacket();
    void handlePacket(Packet* packet);

};

#endif
