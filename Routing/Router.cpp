

#include "Node.h"
#include "Router.h"
#include "Packet.h"
#include <iostream>

using namespace std;

void Router::handlePacket(Packet *){
  cout << "Router is handling packet"; 
  }

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

void Router::addLink(Link * l) {
    this->links.push_back(l);
}
