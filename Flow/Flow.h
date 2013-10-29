

#ifndef FLOW_H
#define FLOW_H

#include<set>


class Host;
class Packet;

/* Class that controlls packet flow between hosts. */
class Flow
{
private:
  int ID;
  int size; // Size of flow, in bytes (totalPackets * 1024 bytes)
  int totalPackets; // number of packets of flow
  int progress; // number of packets successfully transferred
  bool *donePackets; // Array of booleans of packets and transfer state
  std::set<Packet*> outstanding; // set of outstanding packets 
  Host *source;
  Host *destination;
  int packetsSent; // packets sent in last time interval
  int packetsReceived; // packets received in last time interval


public:
  Flow(int in_ID, int in_size, Host *in_source, Host *in_destination);
  ~Flow();


  /* Handles packet when it gets to host. May or may not contain ack. */
  void packetHandler(Packet *packet, Host *host);
  /* Generates new packet from source host to desintation */
  Packet *makePacket();

};





#endif
