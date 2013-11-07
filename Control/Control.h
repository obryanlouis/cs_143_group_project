// Control.h


#ifndef CONTROL_H
#define CONTROL_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <list>
#include <queue>

#include "Flow.h"
//#include "Link.h"
#include "Node.h"
#include "Router.h"
#include "Header.h"

class Link;
class Router;
class Flow;
class Node;

/* Constants */
static unsigned int ROUTING_UPDATE_PERIOD = 6000;
static unsigned int SNAPSHOT_PERIOD = 6000;
static const unsigned int END_PERIOD = 3; // number of events to execute
                                            // after flows done (for data 
                                            // collection)
static const std::string LINK_OCCUPANCY_FILE = std::string("Output/LinkOccupancy.txt");
static const std::string LINK_PACKET_LOSS_FILE = std::string("Output/LinkPacketLoss.txt");
static const std::string LINK_FLOW_RATE_FILE = std::string("Output/LinkFlowRate.txt");



/* Events that get put into the scheduler */
class Event {
private:
    unsigned int time;
    void (*fp)(void*);
        // A function pointer to a function to execute during Event::execute
    void *arg;
        // The argument to the function pointer

public:
    Event(unsigned int in_time, void (*fp)(void*), void *arg)
        :time(in_time)
        ,arg(arg)
        ,fp(fp)
    {}

    ~Event(){}

    unsigned int getTime() const { return time; }
    void execute();
};

/* Used in the scheduler to order elements in list by time.
 */
struct timecomp{
    bool operator() (Event *lhs, Event *rhs){
        return (lhs->getTime() > rhs->getTime());
    }
};

/* Schedules and executes events*/
class Scheduler{
private:
    std::priority_queue<Event*, std::vector<Event*>, timecomp> *events_p;

public:
    Scheduler();
    void initScheduler();
    ~Scheduler();

    //void add(event_t in_type,  unsigned int in_time, void *in_actOn = 0);
    void add(Event* event_p);
    bool doNext();

    void printAndDestroySchedule();
    unsigned int getCurrentTime();
};

/* Master controller. Deals with input and output. 
 * Also includes functions to enact processes that need
 * to occur system-wide.
 */ 
class Controller {
private:
    std::list<Router*> *routers_p;
    std::list<Link*> *links_p;
    std::list<Flow*> *flows_p;
    Scheduler *schedule_p;
    int flowsLeft;
    
public:
    Controller();
    ~Controller();

    void addRouter(Router *router);
    void addLink(Link *link);
    void addFlow(Flow *flow);

    void printSystem();
    void routerUpdate();
    void add(Event *event_p);

    unsigned int getCurrentTime();
    void run();
};

/* Only Scheduler should update the following */
static unsigned int SYSTEM_TIME;

#endif
