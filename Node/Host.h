// Host.h

#ifndef HOST_H
#define HOST_H

#include <iostream>
#include <cassert>

#include "Node.h"
#include "Packet.h"
#include "Link.h"
#include "Flow.h"

class Flow;
class Node;

class Host : public Node {

    Flow *flow_p;
        // the flow the host is associated with (as a source)
    int dataSent;
        // The amount of data sent in the last time interval (bytes)
    int dataReceived;
        // The amount of data received in the last time interval (bytes)

public:
    Host(int in_id, Link *in_link, Flow *in_flow);
        // create an instance of class Host with the give specifications
    ~Host();
        // destroy this instance of Host

    Link* getLink();
        // returns a pointer to the link the host is connected to
    Flow* getFlow();
        // returns a pointer to the flow the host is associated with
    int getDataSent();
        // Returns the amount of data sent in the last time interval (bytes)
    int getDataReceived();
        // Returns the amount of data received in the last time interval (bytes)
	void resetStats();
        // Resets the stats for the next time interval
    void handlePacket(Packet *packet);
        // handle a received packet

};

#endif

