#include "Main.h"

using namespace std;

extern Controller *SYSTEM_CONTROLLER; 

int main( int argc, const char* argv[] ) {

    Flow *flow1;
    Host *host1, *host2;
    Link *link1, *link2;//, *link3, *link4;
    Router *router1;
    host1 = new Host(1);
    host2 = new Host(2);
    flow1 = new Flow(1, 10240, host1, host2);
    router1 = new Router();
    link1 = new Link(1, host1, router1, 10000, 10, 100);
    link2 = new Link(2, router1, host2, 10000, 10, 100);
    //link3 = new Link(3, host2, router1, 10000, 10, 100);
    //link4 = new Link(4, router1, host1, 10000, 10, 100);


    SYSTEM_CONTROLLER = new Controller();

    SYSTEM_CONTROLLER->addLink(link1);
    SYSTEM_CONTROLLER->addLink(link2);
    //SYSTEM_CONTROLLER->addLink(link3);
    //SYSTEM_CONTROLLER->addLink(link4);

    // Give some time for the routing table to stabilize.
    SYSTEM_CONTROLLER->addFlow(flow1, 10);

    SYSTEM_CONTROLLER->addRouter(router1);

    SYSTEM_CONTROLLER->addHost(host1);
    SYSTEM_CONTROLLER->addHost(host2);

    /*RoutingTable routingTable;
    RouterRoutingPacket packet(host1, host2, link1, &routingTable, 1024);
    link1->handlePacket(&packet);
    host1->handlePacket(&packet);*/

    SYSTEM_CONTROLLER->run();
    
	return 0;
}
