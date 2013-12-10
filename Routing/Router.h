// Router.h

#ifndef ROUTER_H
#define ROUTER_H

#include "Link.h"
#include "Node.h"
#include "RoutingTable.h"

#include <cmath>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <map>

class Link;
class RoutingTable;
class Node;
class Host;

class Router : public Node {
    friend void outputRoutingTables(void *args);

	RoutingTable *routingTable_p;
        // the router's routing table
    void updateSingleNode(Host *host, Link *link);
        // Updates the routing table's path to a single host
    void update(RoutingTable *t, double linkWgt, bool &changed, Node *r,
            Link *l);
        // Updates the routing table for a given destination node r
	
public:
    // CONSTRUCTORS AND DESTRUCTORS
    Router(int id);
        // Create an instance of class Router
    Router(int in_id, std::list<Link*> l);
        // Create an instance of class Router with the given specifications

    ~Router();
        // Destroy this instance of Router

    // Functions
	Link *getNextLink(Node *destination);
        // Returns the next link to route to based on a destination
	bool updateRoutingTable(RoutingTable *t, Link *l);
        // Updates the routing table based on received routing table
        // from link l. Returns true if the routing table
        // changed, false otherwise.
    void handlePacket(Packet* packet); 
        // Handles a packet at the router. If it is a routing table update,
        // it is used to update the routing table. If it is a data or
        // acknowledgement packet, it is forwarded to the next node.
    void broadcastRoutingTable();
        // Broadcasts this router's current routing table
        // to all neighbors.




    void debugRoutingTable();
        // Prints the routing table

    virtual std::string infoString();
        // Prints information of the router as follows: "(Router x)",
        // where x is the router's ID

    // DEBUG
    void print();

};

#endif

