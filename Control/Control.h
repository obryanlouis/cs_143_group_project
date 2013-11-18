// Control.h

#ifndef CONTROL_H
#define CONTROL_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <list>
#include <map>
#include <queue>
#include <stdio.h>

#include "Flow.h"
//#include "Link.h"
#include "Node.h"
#include "Router.h"
#include "Header.h"
#include "InputParser.h"

class Link;
class Router;
class Flow;
class Node;

/* Constants */
static unsigned int ROUTING_UPDATE_PERIOD = 60000;
static unsigned int SNAPSHOT_PERIOD = 5;
unsigned int END_PERIOD = 100; // number of events to execute
                                            // after flows done (for data 
                                            // collection)
namespace {
std::string LINK_OCCUPANCY_FILE = std::string("Output/LinkOccupancy.txt");
std::string LINK_PACKET_LOSS_FILE = std::string("Output/LinkPacketLoss.txt");
std::string LINK_FLOW_RATE_FILE = std::string("Output/LinkFlowRate.txt");

std::string FLOW_SEND_FILE = std::string("Output/FlowSend.txt");
std::string FLOW_RECEIVE_FILE = std::string("Output/FlowReceive.txt");
std::string FLOW_RTT_FILE = std::string("Output/FlowRTT.txt");

std::string HOST_SEND_FILE = std::string("Output/HostSend.txt");
std::string HOST_RECEIVE_FILE = std::string("Output/HostReceive.txt");
}

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

    const unsigned int getTime() { return time; }
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
    int                     snapshotTime;
    int                     routingUpdateTime;
    std::list<HostInfo>     hostInfos;
    std::list<RouterInfo>   routerInfos;
    std::list<LinkInfo>     linkInfos;
    std::list<FlowInfo>     flowInfos;
    std::list<Router*>      routers;
    std::list<Link*>        links;
    std::list<Flow*>        flows;
    std::list<Host*>        hosts;
    Scheduler              *schedule_p;
    int                     flowsLeft;
    std::map<int, Host* >   hostsById;
    std::map<int, Router* > routersById;

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
    void setSnapshotTime(int t);
    void setRoutingUpdateTime(int t);

    // DEBUG: Allow other classes to add packets to the list of
    // all packets, and to remove them
    void assertPacketExists(Packet *p);
    void assertNodeExists(Node *n);
    int numTotalPackets();
    void addPacket(Packet *p);
    void removePacket(Packet *p);
    void checkPackets();

    // DEBUG: Other functions
    void printRoutingTables();

    void run(std::string inputFile);

private:
    void initSystem(std::string inputFile);
    Node* getNode(int type, int id, std::map<int, Host* > hostsById,
            std::map<int, Router* > routersById);


};

/* Only Scheduler should update the following */
static unsigned int SYSTEM_TIME;

#endif
