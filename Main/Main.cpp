#include "Main.h"

using namespace std;

extern Controller *SYSTEM_CONTROLLER; 

int main( int argc, const char* argv[] ) {

    Flow *flow1;
    Host *host1, *host2;
    Link *link1, *link2, *link3, *link4, *link5, *link6;
    Router *router1, *router2, *router3, *router4;
    host1 = new Host(1);
    host2 = new Host(2);
    flow1 = new Flow(1, (10240 * 3), host1, host2);
    router1 = new Router(1);
    router2 = new Router(2);
    link1 = new Link(1, host1, router1, 10000, 20, 100);
    link2 = new Link(2, router1, router2, 10000, 10, 100);
    link3 = new Link(3, router2, host2, 10000, 10, 100);
    /*link4 = new Link(4, router2, router4, 10000, 10, 100);
    link5 = new Link(5, router3, router4, 10000, 10, 100);
    link6 = new Link(6, router4, host2, 10000, 10, 100);*/

    SYSTEM_CONTROLLER = new Controller();

    SYSTEM_CONTROLLER->addLink(link1);
    SYSTEM_CONTROLLER->addLink(link2);
    SYSTEM_CONTROLLER->addLink(link3);
    /*SYSTEM_CONTROLLER->addLink(link4);
    SYSTEM_CONTROLLER->addLink(link5);
    SYSTEM_CONTROLLER->addLink(link6);*/

    // Give some time for the routing table to stabilize.
    SYSTEM_CONTROLLER->addFlow(flow1, 100);

    SYSTEM_CONTROLLER->addRouter(router1);
    SYSTEM_CONTROLLER->addRouter(router2);
    /*SYSTEM_CONTROLLER->addRouter(router3);
    SYSTEM_CONTROLLER->addRouter(router4);*/

    SYSTEM_CONTROLLER->addHost(host1);
    SYSTEM_CONTROLLER->addHost(host2);

    SYSTEM_CONTROLLER->run();


    
	return 0;
}
