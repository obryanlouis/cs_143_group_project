
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

public:
	Router() { 
		this->routingTable = new RoutingTable;
	}
	Link *getNextLink(Router *destination);
	Router *getNextRouter(Router *destination);
	void bellmanFord(std::list<Router*> nodes, std::list<Link*> edges);

private:
	RoutingTable *routingTable;
	std::list<Link*> links;
	
	Link* walkBackwards(Router *u, Router *v, std::map<Router *, Router *> *predecessor);
};

#endif
