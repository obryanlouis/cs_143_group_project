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
    : Node(in_id)
{
    this->links = l;
}

Router::~Router() {
    delete this->routingTable_p;
}

void Router::handlePacket(Packet *packet){
    std::cout << "Router is handling packet" << std::endl;

    bool  updated;   // to be used if the packet is for Routing Table Updates

    // Initialize variables to right type (must be done before switch table.)
    RouterRoutingPacket *R = (RouterRoutingPacket *) packet;
    AckPacket *A = (AckPacket *) packet;
    DataPacket *D = (DataPacket *)packet;
    HostRoutingPacket *H = (HostRoutingPacket *) packet;

    switch (packet->getType())
    {
    case Packet::HOSTROUTE:
        // the host is telling this router that it exists 
        this->updateSingleNode(H->getHost(), H->getLink());
        break;
    case Packet::ROUTERROUTE:
        // handing routing table information
        updated = updateRoutingTable(R->getRoutingTable(), R->getLink());
        // If the routing table was updated, broadcast this to all
        // neighbors.
        if (updated) {
            this->broadcastRoutingTable();
        }

        break;
    case Packet::ACK:
        // handle acknowledgement packets: same as data packets
        break;
    case Packet::DATA:
        // handle data packets
        (getNextLink(D->getDestination()))->handlePacket(packet);
        break;
    default:
        std::cout << "INVALID PACKET TYPE" << std::endl;
        assert(0);
        break;
    }
}

void Router::broadcastRoutingTable() {
    for (std::list<Link* >::iterator it = this->links.begin();
            it != this->links.end(); it++)
    {
        RouterRoutingPacket *newRoutingPacket 
            = new RouterRoutingPacket(NULL, NULL, *it, this->routingTable_p);
        (*it)->handlePacket(newRoutingPacket);
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


void Router::updateSingleNode(Host *host, Link *link) {
    // The router is directly connected to this host through the
    // link. Just set the distance to this host to be the link's
    // delay.
    std::pair<int, Link*> pair = std::make_pair<int, Link*>(
            link->getDelay(), link);
    (*this->routingTable_p)[host] = pair;
}
