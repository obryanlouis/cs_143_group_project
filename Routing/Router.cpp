// Router.cpp

#include "Router.h"
#include "Node.h"
#include "Packet.h"
#include <iostream>

Router::Router() { 
    // Make a new routing table
    this->routingTable_p = new RoutingTable();
    // Add an entry to the routing table saying that the distance to this router
    // is 0.
    (*this->routingTable_p)[this] = std::make_pair<int, Link*>(0, NULL);
}

Router::Router(int in_id, std::list<Link*> l)
    : Node(in_id, l)
{}

Router::~Router() {
    delete this->routingTable_p;
}

void Router::handlePacket(Packet *packet, Link *link){
    std::cout << "Router is handling packet" << std::endl;

    bool  updated;   // to be used if the packet is for Routing Table Updates
    Link *nextLink;  // to be used for handling data packets

    switch (packet->getType())
    {
    case Packet::ROUTE:
        // handing routing table information
        updated = updateRoutingTable(packet->getRoutingTable(), link);
        if (updated == 0) {
            // successfully updated routing table
        } else {
            // failed to update...throw some exception?
        }

            // the 0 should be the Link * we received this packet from
            // will depend on implementation
        break;
    case Packet::ACK:
        // handle acknowledgement packets: same as data packets
    case Packet::DATA:
        // handle data packets
        nextLink = getNextLink(packet->getDestination());
        nextLink->handlePacket(packet);
        break;
    default:
        std::cout << "INVALID PACKET TYPE" << std::endl;
        assert(0);
        break;
    }
}

Link* Router::getNextLink(Node *destination) {
	return this->routingTable_p->nextLink(destination);
}

Node* Router::getNextNode(Node *destination) {
	return this->routingTable_p->nextNode(destination);
}


/*  Updates the routing table of this router based on a neighbor's table   */
bool Router::updateRoutingTable(RoutingTable *t, Link *l) {
    bool changed = false;
    for (std::map<Node*, std::pair<int, Link*> >::iterator it = t->mapping.begin();
        it != t->mapping.end(); ++it) {
        Node *r = it->first;
        // If this routing table doesn't have an entry for this node, or its distance
        // is greater than the calculated distance, then create/update the entry.
        // Prevent paths that would go back through this router.
        if ((routingTable_p->mapping.count(r) == 0 ||
            (*routingTable_p)[r].first + l->getDelay() > t->mapping[r].first)
            && t->mapping[r].second != l) {
            (*routingTable_p)[r].first = t->mapping[r].first + l->getDelay();
            (*routingTable_p)[r].second = l;
            changed = true;
        }
    }
    return changed;
}

void Router::addLink(Link * l) {
    this->links.push_back(l);
}

