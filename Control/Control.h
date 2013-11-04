// Control.h


#ifndef CONTROL_H
#define CONTROL_H

#include <stdlib.h>
#include <list>
#include <queue>

#include "Flow.h"
#include "Link.h"
#include "Node.h"
#include "Router.h"

/* Constants */
static unsigned int ROUTING_UPDATE_PERIOD = 6000;
static unsigned int SNAPSHOT_PERIOD = 6000;
static const unsigned int END_PERIOD = 100; // number of events to execute
                                            // after flows done (for data 
                                            // collection)


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

  void printSystem();
  void routerUpdate();

  void run();
};

/* Pointer to the master controller. 
 * This is ONLY set by the Controller constructor. 
 */
static Controller *SYSTEM_CONTROLLER; 


#endif
