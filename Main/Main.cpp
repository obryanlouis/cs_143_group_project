#include "Main.h"

using namespace std;
int main( int argc, const char* argv[] ) {

    SYSTEM_CONTROLLER = new Controller();    

    Flow *flow1, *flow2;
    Host *host1, *host2;
    Link *link1, *link2, *link3, *link4;
    Router *router1;
    link1 = new Link(1, host1, router1, 10000, 10, 100);
    link2 = new Link(2, router1, host2, 10000, 10, 100);
    link3 = new Link(3, host2, router1, 10000, 10, 100);
    link4 = new Link(4, router1, host1, 10000, 10, 100);
    host1 = new Host(1, flow1);
    host2 = new Host(2, flow2);
    flow1 = new Flow(1, 100, host1, host2);
    flow2 = new Flow(2, 100, host2, host1);
    router1 = new Router();
    router1->addLink(link2);
    router1->addLink(link4);

    SYSTEM_CONTROLLER->run();
    
	return 0;
}
