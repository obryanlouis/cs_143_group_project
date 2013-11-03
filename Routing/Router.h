
#ifndef ROUTER_H
#define ROUTER_H

#include "Link.h"
#include "RoutingTable.h"

#include <stdlib.h>
#include <math.h>
#include <list>
#include <map>

class Link;
class RoutingTable;

class Router {

	RoutingTable *routingTable_p;
        // the router's routing table
	std::list<Link*> links;
        // the links the router is connected to
	
	Link* walkBackwards(
            Router *u, Router *v, std::map<Router *, Router *> *predecessor);

public:
    Router();
        // Create an instance of class Router
	Link *getNextLink(Router *destination);
        // Returns the next link to route to based on a destination
	Router *getNextRouter(Router *destination);
        // Returns the next router to route to based on destination
	void bellmanFord(std::list<Router*> nodes, std::list<Link*> edges);
	bool updateRoutingTable(RoutingTable *t, Link *l);
        // Updatesthe routing table based on received routing table
        // from link l. Returns true if updated, false otherwise.
    void addLink(Link *l);
        // 

};

Router::Router() { 
    this->routingTable = new RoutingTable();
}

#endif
