// Flow.h

#ifndef FLOW_H
#define FLOW_H

#include <set>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <string>
#include <cfloat>

#include "CongestionAlgorithm.h"
#include "Control.h"
#include "Packet.h"
#include "Host.h"

class DataPacket;
class AckPacket;
class Host;

static int FLOW_END = -1;
static int FLOW_MAINTENANCE_PERIOD = 1;

/* Class that controls packet flow between hosts. */
class Flow {
    friend void makeAndSendPacket(int id, Flow *flow);
    friend void maintainFlowCallback(void *arg);
    int flowId;
        // The id of the flow
    int size;
        // Size of flow, in bytes (totalPackets * 1024 bytes)
    int totalPackets;
        // Number of packets in the flow
    int progress;
        // Number of packets successfully transferred so far
    std::map<int, double> packets;
        // A map that will keep track of whether or not packets have
        // been successfully sent and received.
        // The key is the packet Id.
        // The value is 0 if the packet has not been sent and the 
        // time of timeout otherwise.      
    std::set<int> acks;
        // A set that keeps track of which data packets have been 
        // recieved at the destination 
    int outstanding;
        // number of outstanding packets
    Host *source;
        // The source host of this flow
    Host *destination;
        // The destination host of this flow
    int dataSent;
        // data sent in last time interval
    int dataReceived;
        // data received in last time interval
    CongestionAlgorithm *congestionAlgorithm_p;

public:
    Flow(int in_ID, int in_size, Host *in_source, Host *in_destination,
            CongestionAlgorithmType algorithm);
        // Create an object of class Flow with the given specifications
    ~Flow();
        // Destroy this instance of Flow

    void startFlow(double startTime);
        // creates first event for this flow 
    void sendNewPacket(DataPacket *p, double timeOut);
        // takes packet created by flow and updates internals
    
        
    double getPacketTime(int id);
    int getNextUnrecieved();
 
    void handlePacket(AckPacket *p);
        // Handles an incoming packet acknowledgement 
    AckPacket *atDest(DataPacket *p);
        // generates an AckPacket based off of TCP congestion alg
        // for when DataPacket recieved at Destination.
    void resetPackets(int id); 
        // make packets of >= id behave as though they have not been
        // sent
    int nextHostPacket();


    void resetStats();
        // Reset the stats in the last time interval.
    void updateDataSent(int bytes);
        // Update the data sent in the last time interval by adding
        // bytes to the dataSent field.
    void updateDataReceived(int bytes);
        // Update the data received in the last time interval by adding
        // bytes to the dataReceived field.
    Host *getStart();
    Host *getDestination();
    int getId();
    std::string infoString();
    double getStats(std::string stat, int period);
    int getTotalPackets();

};


#endif
