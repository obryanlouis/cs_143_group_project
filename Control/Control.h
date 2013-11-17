// Control.h


#ifndef CONTROL_H
#define CONTROL_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <list>
#include <queue>
#include <stdio.h>

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
static unsigned int ROUTING_UPDATE_PERIOD = 60000;
static unsigned int SNAPSHOT_PERIOD = 5;
static const unsigned int END_PERIOD = 100; // number of events to execute
                                            // after flows done (for data 
                                            // collection)
static const std::string LINK_OCCUPANCY_FILE = std::string("Output/LinkOccupancy.txt");
static const std::string LINK_PACKET_LOSS_FILE = std::string("Output/LinkPacketLoss.txt");
static const std::string LINK_FLOW_RATE_FILE = std::string("Output/LinkFlowRate.txt");

static const std::string FLOW_SEND_FILE = std::string("Output/FlowSend.txt");
static const std::string FLOW_RECEIVE_FILE = std::string("Output/FlowReceive.txt");
static const std::string FLOW_RTT_FILE = std::string("Output/FlowRTT.txt");

static const std::string HOST_SEND_FILE = std::string("Output/HostSend.txt");
static const std::string HOST_RECEIVE_FILE = std::string("Output/HostReceive.txt");

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
    ~Scheduler();

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
    std::list<Host*> *hosts_p;
    Scheduler *schedule_p;
    int flowsLeft;

    friend void makePlots();

    // DEBUG: Keep track of all packets
    std::map<Packet *, bool> packets;
    
public:
    Controller();
    ~Controller();

    void addRouter(Router *router);
    void addLink(Link *link);
    void addFlow(Flow *flow, unsigned int startTime);
    void addHost(Host *host);
    unsigned int getCurrentTime();
    void decrementFlowsLeft();
 
    void printMySystem();
    void updateMyRouters();
    void add(Event *event_p);

    // DEBUG: Allow other classes to add packets to the list of
    // all packets, and to remove them
    void assertPacketExists(Packet *p);
    void assertNodeExists(Node *n);

    void run();

private:
    void initSystem();

};

/* Only Scheduler should update the following */
static unsigned int SYSTEM_TIME;

#endif
