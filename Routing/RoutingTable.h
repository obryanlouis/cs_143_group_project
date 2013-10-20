

#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include "Link.h"

#include <map>
#include <cstddef>
#include <utility>

class Link;
class Router;

class RoutingTable
{
public:
	friend class Router;

	std::pair<int, Link*> & operator[] (Router *r) { return mapping[r]; }

	Link* nextLink(Router *r);
	Router* nextRouter(Router *r);
	
private:
	std::map<Router*, std::pair<int, Link*> > mapping;

};

#endif
