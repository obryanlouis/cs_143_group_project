

#ifndef LINK_H
#define LINK_H

#include<queue>
#include "Node.h"

class Node;
class Packet;


/* Link class which controls packet transfer between routers. */
class Link {
	
private:
  int ID;
	Node *end1;
	Node *end2;
  int capacity; // maximum capacity (in bytes)
  int delay; // time delay for travel through link (ignoring in/out time)
  std::queue<Packet*> buffer; // FIFO queue of packets
  int packetsSent; // used for stat keeping
  int packetsReceived; // used for stat keeping 
	int	traffic;	// change this
	int length;	// can change this


public:
  Link (){}
  Link(int in_ID, Node *in_end1, Node *in_end2, int in_capacity,
    int in_delay):
    ID(in_ID),
    end1(in_end1),
    end2(in_end2),
    capacity(in_capacity),
    delay(in_delay),
    packetsSent(0),
    packetsReceived(0) 
  {}

  ~Link() { }
    
	int getTraffic() { return this->traffic; }
	void resetTraffic() { this->traffic = 0; }
	int getLength() { return this->length; }
	Node *getEnd1() { return this->end1; }
	Node *getEnd2() { return this->end2; }
  void setEnds(Node *n1, Node *n2);

  Packet* popPacket();
  void pushPacket(Packet *in_packet);

};

#endif
