#include <iostream>

#include "Control.h"
#include "Flow.h"
#include "Link.h"
#include "Node.h"
#include "Router.h"
#include "RoutingTable.h"

int main( int argc, const char* argv[] ) {

    Event *my_event = new Event(UPDATE_FLOWS, 13);

    Scheduler *my_scheduler = new Scheduler();
    my_scheduler->add(my_event);
    my_scheduler->add(UPDATE_LINKS, 2);
    my_scheduler->add(UPDATE_LINKS, 5);
    my_scheduler->printAndDestroySchedule();

    Controller *my_controller = new Controller();
    my_controller->run();
	return 0;
}
