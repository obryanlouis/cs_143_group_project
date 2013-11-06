#include "Main.h"

int main( int argc, const char* argv[] ) {

    SYSTEM_CONTROLLER = new Controller();    

    Router *first = new Router();
    Router *second = new Router();

    Link *link = new Link();
    RoutingTable *rt = new RoutingTable();

    RoutingPacket *packet = new RoutingPacket(first, second, rt, link);
        
    SYSTEM_CONTROLLER->run();
    
	return 0;
}
