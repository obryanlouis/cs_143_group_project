// RoutingTable.h

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
    friend void outputRoutingTables(void *args);
    friend class Router;
    RoutingTable();
    RoutingTable(RoutingTable *old);
    ~RoutingTable();

	std::pair<double, Link*> & operator[] (Node *r) { return mapping[r]; }

	Link* nextLink(Node *r);
	Node* nextNode(Node *r);
    bool containsEntry(Node *node);
        // Determines whether or not the routing table has an entry
        // for this node.

    // DEBUG
    void print();
	
private:
	std::map<Node*, std::pair<double, Link*> > mapping;

};

#endif
