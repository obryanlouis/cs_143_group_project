// Router.cpp

#include "Router.h"
#include "Node.h"
#include "Packet.h"
#include <iostream>


void Router::handlePacket(Packet *packet){
    std::cout << "Router is handling packet" << std::endl;

    switch (packet->getType())
    {
    case Packet::ROUTE:
        // handing routing table information
        bool updated = updateRoutingTable(packet->getRoutingTable(), 0);
            // the 0 should be the Link * we received this packet from
            // will depend on implementation
        break;
    case Packet::ACK:
        // handle acknowledgement packets: same as data packets
    case Packet::DATA:
        // handle data packets
        Link *nextLink = getNextLink(packet->getDestination());
        nextLink->handlePacket(packet);
        break;
    default:
        std::cout << "INVALID PACKET TYPE" << std::endl;
        assert(0);
        break;
    }
}


/*	Walks backwards from u to v on a predecessor map of Routers	*/
Link* walkBackwards(Node *u, Node *v, map<Router *, Router *> predecessor);

Link* Router::getNextLink(Node *destination) {
	return this->routingTable->nextLink(destination);
}

Node* Router::getNextNode(Node *destination) {
	return this->routingTable->nextNode(destination);
}


/*  Updates the routing table of this router based on a neighbor's table   */
bool Router::updateRoutingTable(RoutingTable *t, Link *l) {
    bool changed = false;
    for (std::map<Node*, std::pair<int, Link*> >::iterator it = t->mapping.begin();
        it != t->mapping.end(); ++it) {
        Node *r = it->first;
        if ((*routingTable)[r].first > t->mapping[r].first) {
            (*routingTable)[r].first = t->mapping[r].first;
            (*routingTable)[r].second = l;
            changed = true;
        }
    }
    return changed;
}


Link* Router::walkBackwards(Node *u, Node *v, map<Node *, Node *> *predecessor)
{
	/*	Loop until the predecessor is the source	*/
	Node *finder = v;
	while ((*predecessor)[finder] != u) {
		finder = (*predecessor)[finder];
	}

	/*	Find the link that corresponds to the connection between these
		routers	*/
	for (list<Link*>::iterator it = u->links.begin(); 
		it != u->links.end(); ++it) {
			if ((*it)->getEnd1() == finder || (*it)->getEnd2() == finder) {
				return *it;
			}
	}

	/*	If the execution gets here, there is no link between the router
		and its predecessor :O	*/
	throw "No Link available";
}

void Router::addLink(Link * l) {
    this->links.push_back(l);
}
