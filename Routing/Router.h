
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

private:
	RoutingTable *routingTable;
	Link* walkBackwards(Node *u, Node *v, std::map<Node *, Node *> *predecessor);
	
public:
    // Constructor and destructors 
	Router() { 
		this->routingTable = new RoutingTable;
	}
    Router(int in_id):
    Node(in_id) {}
    ~Router() { delete this->routingTable; }

    /* Basic manipulating thingies */

    /* Functions */
	Link *getNextLink(Node *destination);
	Node *getNextNode(Node *destination);
	void bellmanFord(std::list<Node*> nodes, std::list<Link*> edges);
	bool updateRoutingTable(RoutingTable *t, Link *l);
    void addLink(Link *l);      
    virtual void handlePacket(Packet* packet); 
};

#endif
