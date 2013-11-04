// Control.h


#ifndef CONTROL_H
#define CONTROL_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <list>
#include <queue>

#include "Flow.h"
#include "Link.h"
#include "Node.h"
#include "Router.h"

/* Constants */
static const unsigned int ROUTING_UPDATE_PERIOD = 6000;
static const unsigned int SNAPSHOT_PERIOD = 6000;
static const unsigned int END_PERIOD = 100; // number of events to execute
                                            // after flows done (for data 
                                            // collection)
static const std::string LINK_OCCUPANCY_FILE = std::string("Output/LinkOccupancy.txt");
static const std::string LINK_PACKET_LOSS_FILE = std::string("Output/LinkPacketLoss.txt");
static const std::string LINK_FLOW_RATE_FILE = std::string("Output/LinkFlowRate.txt");

/* Different type of events */
enum event_t{
    UPDATE_ROUTING,
    PRINT_STATS,
    UPDATE_FLOWS,
    UPDATE_LINKS
};

/* Events that get put into the scheduler */
class Event {
private:
    event_t type;
    unsigned int time;
    void *actOn;

public:
    Event(event_t in_type, unsigned int in_time, void *in_actOn = 0)
        :type(in_type)
        ,time(in_time)
        ,actOn(in_actOn)
    {}

    ~Event(){}

    unsigned int getTime() const { return time; }
    event_t getType() const { return type;}
    Event *execute();
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

    void add(event_t in_type,  unsigned int in_time, void *in_actOn = 0);
    void add(Event* event_p);
    bool doNext();

    void printAndDestroySchedule();
    unsigned int getCurrentTime();
};

/* Master controller for everything */ 
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

    void run();
    void statsSnapshot();
        // Writes the relevant stats to files so that they can be graphed
};


#endif
