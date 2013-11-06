#include "Main.h"

using namespace std;
int main( int argc, const char* argv[] ) {

    SYSTEM_CONTROLLER = new Controller();    

    Router *first = new Router();
    Router *second = new Router();

    Link *link = new Link();
    RoutingTable *rt = new RoutingTable();

    RoutingPacket *packet = new RoutingPacket(first, second, rt, link);

    cout << first <<endl << second <<endl << link << endl <<rt <<endl<< packet << SYSTEM_CONTROLLER;
        
    SYSTEM_CONTROLLER->run();
    
	return 0;
}
