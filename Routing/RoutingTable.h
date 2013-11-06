

#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include "Link.h"

#include <map>
#include <cstddef>
#include <utility>

class Link;
class Node;

class RoutingTable
{
public:
    friend class Router;
    RoutingTable();
    RoutingTable(RoutingTable *old);
    ~RoutingTable();

	std::pair<int, Link*> & operator[] (Node *r) { return mapping[r]; }

	Link* nextLink(Node *r);
	Node* nextNode(Node *r);
	
private:
	std::map<Node*, std::pair<int, Link*> > mapping;

};

#endif
