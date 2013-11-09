// Flow.h

#ifndef FLOW_H
#define FLOW_H

#include <set>
#include <vector>

#include "Packet.h"

class Host;
class Packet;

/* Class that controls packet flow between hosts. */
class Flow {
    int flowId;
        // The id of the flow
    int size;
        // Size of flow, in bytes (totalPackets * 1024 bytes)
    int totalPackets;
        // Number of packets in the flow
    int progress;
        // Number of packets successfully transferred so far
    std::vector<bool> donePackets;
        // Vector of booleans of packets and transfer state
    std::set<Packet*> outstanding;
        // set of outstanding packets 
    Host *source;
        // The source host of this flow
    Host *destination;
        // The destination host of this flow
    int dataSent;
        // data sent in last time interval
    int dataReceived;
        // data received in last time interval


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
    Host *getStart();
    Host *getDestination();


};


#endif
