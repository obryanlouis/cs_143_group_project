// Host.h

#ifndef HOST_H
#define HOST_H

#include <iostream>
#include <cassert>

#include "Node.h"
#include "Packet.h"
#include "Link.h"
#include "Flow.h"
#include "Control.h"

class Flow;
class Node;
class Controller;

extern Controller *SYSTEM_CONTROLLER;

class Host : public Node {

    Flow *flow_p;
        // the flow the host is associated with (as a source)
    Flow *flow2_p;
        // the flow the host is associated with (as a destination)
    int dataSent;
        // The amount of data sent in the last time interval (bytes)
    int dataReceived;
        // The amount of data received in the last time interval (bytes)

public:
    Host(int in_id);
        // create an instance of class Host with the give specifications
    ~Host();
        // destroy this instance of Host

    Link* getLink();
        // returns a pointer to the link the host is connected to
    void setFlow(Flow *flow);
    void setFlow2(Flow *flow);
    Flow* getFlow();
        // returns a pointer to the flow the host is associated with
    Flow* getFlow2();
    int getDataSent();
        // Returns the amount of data sent in the last time interval (bytes)
    int getDataReceived();
        // Returns the amount of data received in the last time interval (bytes)
	void resetStats();
        // Resets the stats for the next time interval
    void handlePacket(Packet *packet);
        // handle a received packet
    void setLink(Link *link);
        // Set the link that the host is connected to

    double getStats(std::string stat, int period);
        // Returns the stat requested by the stat string.
        // Options: "send rate" and "receive rate"

    virtual std::string infoString();
        // Returns a string formatted to describe the Host.

};

#endif

