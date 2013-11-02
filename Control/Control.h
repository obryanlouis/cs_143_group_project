
#ifndef CONTROL_H
#define CONTROL_H

#include "Link.h"
#include "Router.h"

#include <stdlib.h>
#include <list>
#include <set>
#include <stdio>
#include <iostream>
#include <fstream>


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
  int ID;
  unsigned int time;

public:
  Event(event_t in_type, int in_ID, unsigned int in_time):
    type(in_type),
    ID(in_ID),
    time(in_time)
    {}

  ~Event(){}

  execute();
};

/* Used in the scheduler to order elements in list by time.
 */
struct timecomp{
  bool operator() (const Event *lhs, const Event *rhs) const{
    return lhs->time < rhs->time;
  }
};

/* Schedules and executes events*/
class Scheduler{
private:
  std::multiset<Event*, timecomp> events;

public:
  Scheduler();
  ~Scheduler();
  add(event_t in_type, int in_ID, unsigned int in_time);
  doNext();
  printSchedule();
};


/* Master controller for everything */ 
class Controller {
private:
  std::list<Router*> routers;
  std::list<Link*> links;
  std::list<Flows*> flows;
  Scheduler *schedule;

public:
  Controller();
  ~Controller();

  run();
  debug();

};


#endif
