
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
#include <sstream>
#include <climits>

#include "CongestionAlgorithm.h"
#include "Vegas.h"
#include "Cubic.h"
#include "Control.h"
#include "Packet.h"
#include "Host.h"

class DataPacket;
class AckPacket;
class Host;

static int FLOW_END = -1;
static int FLOW_MAINTENANCE_PERIOD = 1;

// Class that controls packet flow between hosts.
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
    double totalPacketDelay;
        // The total sum of delays of packets received in last time interval
    int packetsReceived;
        // The number of packets received in the last time interval
    CongestionAlgorithm *congestionAlgorithm_p;
    std::list<double> rtts;
    double totalLinkLoss;
    int totalPacketsRecieved;
    double start;
    bool done;

public:
    Flow(int in_ID, int in_size, Host *in_source, Host *in_destination,
            CongestionAlgorithmType algorithm);
        // Create an object of class Flow with the given specifications
    ~Flow();
        // Destroy this instance of Flow

    double getStartTime();
    int getProgress();
    void startFlow(double startTime);
        // creates first event for this flow
    void sendNewPacket(DataPacket *p, double timeOut);
        // takes packet created by flow and updates internals
    
        
    double getPacketTime(int id);
        // Returns the packets with the given id's start time
    int getNextUnrecieved();
        // Returns the id of the next unreceived packet
 
    void handlePacket(AckPacket *p);
        // Handles an incoming packet acknowledgement
    AckPacket *atDest(DataPacket *p);
        // generates an AckPacket based off of TCP congestion alg
        // for when DataPacket recieved at Destination.
    void resetPackets(int id);
        // make packets of >= id behave as though they have not been
        // sent
    int nextHostPacket();
    int getDroppedPacket();


    void resetStats();
        // Reset the stats in the last time interval.
    void updateDataSent(int bytes);
        // Update the data sent in the last time interval by adding
        // bytes to the dataSent field.
    void updateDataReceived(int bytes);
        // Update the data received in the last time interval by adding
        // bytes to the dataReceived field.
    void updateTotalPacketDelay(double delay);
        // Update the total packet delay in the last time interval by adding
        // the delay of the last received packet. Also updates the number
        // of packets received
    Host *getStart();
        // Returns the source host of the flow
    Host *getDestination();
        // Returns the destination host of the flow
    int getId();
        // Returns the id of the flow
    std::string infoString();
        // Returns a string formatted to describe the flow
    double getStats(std::string stat, int period);
        // Returns the stat requested by the stat string
        // Options: "send rate", "receive rate", "delay", "window",
        //          "thresh", "outstanding"
    int getTotalPackets();
        // Returns the total number of packets in this flow

    // DEBUG
    void checkAllRecieved(int sendNext);
    void printRemainingPacketIds();
    double getRTT();
    void updateRTT(double rtt);

};


#endif 
