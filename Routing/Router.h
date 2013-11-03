// Router.h

#ifndef ROUTER_H
#define ROUTER_H

#include "Link.h"
#include "Node.h"
#include "RoutingTable.h"

#include <stdlib.h>
#include <math.h>
#include <list>
#include <map>

class Link;
class RoutingTable;
class Node;

/*! Router class */
class Router : public Node {

	RoutingTable *routingTable_p;
        // the router's routing table

	Link* walkBackwards(Node *u, Node *v, std::map<Node *, Node *> *predecessor);
	
public:
    // CONSTRUCTORS AND DESTRUCTORS
    Router();
        // Create an instance of class Router
    Router(int in_id);
        // Create an instance of class Router with the given id

    ~Router();

    // Functions
	Link *getNextLink(Node *destination);
        // Returns the next link to route to based on a destination
	Node *getNextNode(Node *destination);
        // Returns the next router to route to based on destination
	void bellmanFord(std::list<Node*> nodes, std::list<Link*> edges);
	bool updateRoutingTable(RoutingTable *t, Link *l);
        // Updates the routing table based on received routing table
        // from link l. Returns true if updated, false otherwise.
    void addLink(Link *l);
        // Adds a link to the router
    void handlePacket(Packet* packet); 
};

Router::Router() { 
    this->routingTable_p = new RoutingTable();
}

Router::Router(int in_id)
    : Node(in_id)
{}

Router::~Router() {
    delete this->routingTable_p;
}

#endif

