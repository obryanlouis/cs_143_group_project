

#ifndef HOST_H
#define HOST_H

#include "Link.h"
#include "Flow.h"

class Host : public Node {

    Link *link_p;
        // the link the host is connected to
    Flow *flow_p;
        // the flow the host is associated with (as a source)

public:
    Host(int in_id, Link *in_link, Flow *in_flow);
        // create an instance of class Host with the give specifications
    ~Host();
        // destroy this instance of Host

    Link* getLink();
        // returns a pointer to the link the host is connected to
    Flow* getFlow();
        // returns a pointer to the flow the host is associated with
    void handlePacket(Packet *packet);
        // handle a received packet

};

#endif
