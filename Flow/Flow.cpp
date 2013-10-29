


#include "Flow.h"
#include <set>

static const int INFO_PACKET_SIZE = 1024;

/*! Initializer !*/
Flow::Flow(int in_ID, int in_size, Host *in_source, Host *in_destination):
  ID(in_ID),
  size(in_size),
  source(in_source),
  destination(in_destination),
  progress(0),
  packetsSent(0),
  packetsReceived(0)
  {
    // Calculate the number of packets we need based on size of flow 
    this->totalPackets = (in_size + 1023) / INFO_PACKET_SIZE;
    this->donePackets = new bool[this->totalPackets];
    for (int i = 0; i < this->totalPackets; i++){
      this->donePackets[i] = false;
    }
  }


/* Handles packet when it gets to host. may or may not contain ack. */
void Flow::packetHandler(Packet *packet, Host* host){}

/* Generates new packet from source host to destination. */ 
Packet* Flow::makePacket(){}
