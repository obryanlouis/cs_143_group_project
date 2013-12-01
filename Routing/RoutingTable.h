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

private:
	std::map<Node*, std::pair<double, Link*> > mapping;
        // The map containing the routing table information. Maps from
        // a destination node to a pair, containing the distance to this
        // node as well as the next link to follow

public:
    friend void outputRoutingTables(void *args);
    friend class Router;
    RoutingTable();
        // Create an empty RoutingTable
    RoutingTable(RoutingTable *old);
        // Copy an object of class RoutingTable
    ~RoutingTable();
        // Delete this instance of RoutingTable

	std::pair<double, Link*> & operator[] (Node *r) { return mapping[r]; }
        // operator[] to access elements of the routing table

	Link* nextLink(Node *r);
        // Returns the next link a packet should follow based on its
        // destination node
	Node* nextNode(Node *r);
        // Returns the next node a packet should go to based on its
        // destination node
    bool containsEntry(Node *node);
        // Determines whether or not the routing table has an entry
        // for this node.

    // DEBUG
    void print();
	
};

#endif

