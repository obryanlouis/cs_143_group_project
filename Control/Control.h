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

#include "CongestionAlgorithm.h"
#include "Flow.h"
#include "Node.h"
#include "Router.h"
#include "InputParser.h"

class Link;
class Router;
class Flow;
class Node;

// Constants
static double ROUTING_UPDATE_PERIOD = 60000;
static double SNAPSHOT_PERIOD = 1;
static double END_PERIOD = 100;
    // number of events to execute
    // after flows done (for data 
    // collection)

namespace {
    // This namespace stores the filenames of where to store the stats
    // being collected on the network.
std::string LINK_OCCUPANCY_FILE = std::string("Output/LinkOccupancy.txt");
std::string LINK_PACKET_LOSS_FILE = std::string("Output/LinkPacketLoss.txt");
std::string LINK_FLOW_RATE_FILE = std::string("Output/LinkFlowRate.txt");

std::string FLOW_SEND_FILE = std::string("Output/FlowSend.txt");
std::string FLOW_RECEIVE_FILE = std::string("Output/FlowReceive.txt");
std::string FLOW_RTT_FILE = std::string("Output/FlowRTT.txt");
std::string FLOW_DELAY_FILE = std::string("Output/FlowDelay.txt");
std::string FLOW_WINDOW_FILE = std::string("Output/FlowWindow.txt");
std::string FLOW_THRESH_FILE = std::string("Output/FlowThresh.txt");
std::string FLOW_OUTSTANDING_FILE = std::string("Output/FlowOutstanding.txt");
std::string FLOW_RENO_FILE = std::string("Output/renodata.txt");
std::string FLOW_VEGAS_FILE = std::string("Output/vegasDiff.txt");

std::string HOST_SEND_FILE = std::string("Output/HostSend.txt");
std::string HOST_RECEIVE_FILE = std::string("Output/HostReceive.txt");
}

// Events that get put into the scheduler
class Event {
private:
    double time;
        // Time the event should be executed
    void (*fp)(void*);
        // A function pointer to a function to execute during Event::execute
    void *arg;
        // The argument to the function pointer

public:
    Event(double in_time, void (*fp)(void*), void *arg)
        :time(in_time)
        ,arg(arg)
        ,fp(fp)
    {}
        // Create an object of class Event with the given specifications

    ~Event(){}
        // Destroy this instance of Event

    const double getTime() { return time; }
        // Returns the time the event is scheduled to be run
    void execute();
        // Execute the event's function pointer with the arguments
};

// Used in the scheduler to order elements in list by time.
struct timecomp {
    bool operator() (Event *lhs, Event *rhs){
        return (lhs->getTime() > rhs->getTime());
    }
};

// Schedules and executes events
class Scheduler {
private:
    std::priority_queue<Event*, std::vector<Event*>, timecomp> *events_p;
        // The priority queue of events to be executed

public:
    Scheduler();
        // Create an object of class Scheduler
    ~Scheduler();
        // Destroy this instance of Scheduler

    void add(Event* event_p);
        // Add an event to the scheduler
    bool doNext();
        // If there are events in the scheduler, finds the next event,
        // executes it, and pops it from the priority queue. Returns
        // true if there are events, and false otherwise.
    void setTime(double time);
        // Sets the current time

    void printAndDestroySchedule();
        // Prints the times of all of the events in the queue, and pops
        // them all off the queue.
    double getCurrentTime();
        // Returns the current time
};

// Master controller. Deals with input and output. 
// Also includes functions to enact processes that need
// to occur system-wide.
class Controller {
private:
    int                     snapshotTime;
        // The time interval at which to collect stats.
    int                     routingUpdateTime;
        // The time interval at which to update routing tables of the routers.
    std::list<HostInfo>     hostInfos;
        // The information of the hosts to create from the input file.
    std::list<RouterInfo>   routerInfos;
        // The information of the routers to create from the input file.
    std::list<LinkInfo>     linkInfos;
        // The information of the links to create from the input file.
    std::list<FlowInfo>     flowInfos;
        // The information of the flows to create from the input file.
    std::list<Router*>      routers;
        // The routers in the network.
    std::list<Link*>        links;
        // The links in the network.
    std::list<Flow*>        flows;
        // The flows in the network.
    std::list<Host*>        hosts;
        // The hosts in the network.
    Scheduler              *schedule_p;
        // The scheduler used to schedule events of the network simulation.
    int                     flowsLeft;
        // Number of flows left running in the network.
    std::map<int, Host* >   hostsById;
        // Map from the host id to the actual host pointer.
    std::map<int, Router* > routersById;
        // Map from the router id to the actual router pointer.
    std::string             inputFile;
        // The filename of the input XML file.
    PlotOptions             plotOptions;

    friend void makePlots(PlotOptions *options);

    // DEBUG: Keep track of all packets
    std::map<Packet *, bool> packets;
    
public:
    Controller();
        // Create an object of class Controller.
    ~Controller();
        // Destroy this instance of Controller.

    void addRouter(Router *router);
        // Adds a router to the network.
    void addLink(Link *link);
        // Adds a link to the network.
    void addFlow(Flow *flow, double startTime);
        // Adds a flow to the network.
    void addHost(Host *host);
        // Adds a host to the network.
    double getCurrentTime();
        // Returns the current time of the simulation.
    void decrementFlowsLeft();
        // Decrements the number of flows left running in the simulation.
 
    void printMySystem();
        // Writes the stats to the files and resets the stats for the
        // next snapshot time interval.
    void updateMyRouters();
        // Updates the routers' routing tables by broadcasting their
        // tables until they stabilize.
    void add(Event *event_p);
        // Adds and event to the scheduler to be run.
    void setSnapshotTime(int t);
        // Sets the snapshot time of the controller.
    void setRoutingUpdateTime(int t);
        // Sets the routing table update time of the controller.
    void setInputFile(std::string inputFile);
        // Sets the input file to use.
    double routerBufferSize();
    void deleteFlow(Packet *p);

    // Output
    double getThroughput();
    double getDataSent();
    double getLinkLoss(double loss);
    int numLinksToPrint();
        // Returns the number of links that will be printed out.
    int numHostsToPrint();
        // Returns the number of hosts that will be printed out.
    int numFlowsToPrint();
        // Returns the number of flows that will be printed out.
    // DEBUG: Allow other classes to add packets to the list of
    // all packets, and to remove them
    void assertPacketExists(Packet *p);
    void assertNodeExists(Node *n);
    int numTotalPackets();
    void addPacket(Packet *p);
    void removePacket(Packet *p);
    void checkPackets();


    // DEBUG: Other functions
    void makeDebugPlots();
    int numberOfPacketsInSystem();
    void printRoutingTables();
    friend void outputRoutingTables(void *args);

    void run();
        // Runs the network simulation.
        // First, parses the input file and creates the network objects.
        // Then, do the first router update and system prints to get the
        // network running.
        // Then, runs until there are no more flows left, or an error occurred.
        // Finishes by creating the plots from the stats collected.
    void initSystem();
        // Initializes the system.
        // First, it parses the input file to get the information necessary
        // to create the network objects.
        // Then, it creates the objects as well as setting the snapshot
        // time and routing table update time.
        // Then, it initializes the flows, and finishes off by removing
    void initRoutingTables();
        // Initializes the routing tables.
        // old stats files.

private:
    Node* getNode(int type, int id, std::map<int, Host* > hostsById,
            std::map<int, Router* > routersById);


};

// Only Scheduler should update the following
static double SYSTEM_TIME;

#endif

