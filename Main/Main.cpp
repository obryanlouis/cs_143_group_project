#include "Main.h"
#include <iostream>
#include <string>
#include <list>

using namespace std;

extern Controller *SYSTEM_CONTROLLER; 

int main(int argc, char **argv) {
    // input should just have one argument, and it should be the xml file
    // containing the specification

    // DEBUG: Uncomment these lines when we want to get a file from a
    // specific location.

    /*if (argc != 2) {
        cout << "usage: read_input inputFile" << endl;
        exit(1);
    }

    char* input = argv[1];*/

    // DEBUG: use a default file input.xml
    string input = "input.xml";

    // Initialize the System Controller
    SYSTEM_CONTROLLER = new Controller();

    SYSTEM_CONTROLLER->run(input);
              
#if 0
    // print some stuff
    cout << "snapshot time:     " << snapshotTime      << endl;
    cout << "routingUpdateTime: " << routingUpdateTime << endl;
    cout << "There are " << hosts.size() << " hosts:" << endl;
    for (list<HostInfo>::iterator it = hosts.begin();
         it != hosts.end(); it++)
    {
        cout << "    "
             << "id="
             << it->hostId
             << endl;
    }
    cout << "There are " << routers.size() << " routers:" << endl;
    for (list<RouterInfo>::iterator it = routers.begin();
         it != routers.end(); it++)
    {
        cout << "   "
             << "id="
             << it->routerId
             << endl;
    }
    cout << "There are " << links.size() << " links:" << endl;
    cout << "Type 0 = host;  type 1 = router" << endl;
    for (list<LinkInfo>::iterator it = links.begin();
         it != links.end(); it++)
    {
        cout << "    " << "id=" << it->linkId << endl
             << "    " << "rate=" << it->linkRate << endl
             << "    " << "delay=" << it->linkDelay << endl
             << "    " << "bufferSize=" << it->bufferSize << endl
             << "    " << "node1Type=" << it->node1Type << endl
             << "    " << "node1Id=" << it->node1Id << endl
             << "    " << "node2Type=" << it->node2Type << endl
             << "    " << "node2Id=" << it->node2Id << endl;
    }
    cout << "There are " << flows.size() << " flows:" << endl;
    for (list<FlowInfo>::iterator it = flows.begin();
         it != flows.end(); it++)
    {
        cout << "    " << "id=" << it->flowId << endl
             << "    " << "sourceId=" << it->sourceId << endl
             << "    " << "destinationId=" << it->destinationId << endl
             << "    " << "flowSize=" << it->flowSize << endl
             << "    " << "startTime=" << it->startTime << endl;
    }
#endif

    return 0;
}
