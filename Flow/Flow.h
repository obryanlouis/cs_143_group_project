// Flow.h

#ifndef FLOW_H
#define FLOW_H

#include <set>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>

#include "Control.h"
#include "Header.h"
#include "Packet.h"
#include "Host.h"

class DataPacket;
class AckPacket;
class Host;

static int FLOW_END = -1;
static int FLOW_MAINTENANCE_PERIOD = 5;

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
    std::map<int, int> packets;
        // A map that will keep track of whether or not packets have
        // been successfully sent and received. The key is the 
        // packet id and the value is 1 if the packet has been
        // sucessfully received, and 0 otherwise.
    std::vector<DataPacket *> outstanding;
        // set of outstanding packets 
    Host *source;
        // The source host of this flow
    Host *destination;
        // The destination host of this flow
    int dataSent;
        // data sent in last time interval
    int dataReceived;
        // data received in last time interval
    unsigned int timeout;
        // the timeout for packets. if this has expired, then the
        // flow should resend the packet
    int windowSize;
    int getNextPacketId();
    void maintain();

public:
    Flow(int in_ID, int in_size, Host *in_source, Host *in_destination);
        // Create an object of class Flow with the given specifications
    ~Flow();
        // Destroy this instance of Flow

    void resetStats();
        // Reset the stats in the last time interval.
    void updateDataSent(int bytes);
        // Update the data sent in the last time interval by adding
        // bytes to the dataSent field.
    void updateDataReceived(int bytes);
        // Update the data received in the last time interval by adding
        // bytes to the dataReceived field.
    void handlePacket(AckPacket *p);
        // Handles an incoming packet acknowledgement
    Host *getStart();
    Host *getDestination();
};


#endif
