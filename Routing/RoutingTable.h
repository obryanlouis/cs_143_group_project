

#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include "Link.h"
#include "Router.h"

#include <map>

class Link;
class Router;

class RoutingTable
{
public:
	friend class Router;

	Link& operator[] (Router *r) { return *mapping[r]; }

	Link* nextLink(Router *r);
	Router* nextRouter(Router *r);
private:
	std::map<Router*, Link*> mapping;

};

#endif