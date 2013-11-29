// Router.cpp

#include "Router.h"
#include "Node.h"
#include "Packet.h"
#include "Control.h"
#include <iostream>

extern Controller *SYSTEM_CONTROLLER;

Router::Router(int id) { 
    // Make a new routing table
    this->routingTable_p = new RoutingTable();
    // Add an entry to the routing table saying that the distance to this router
    // is 0.
    (*this->routingTable_p)[this] = std::make_pair<double, Link*>(0, NULL);
    // Set the id of this router
    this->nodeId = id;
}

Router::Router(int in_id, std::list<Link*> l)
    : Node(in_id)
{
    this->links = l;
}

Router::~Router() {
    delete this->routingTable_p;
}

std::string Router::infoString(){
    std::stringstream ss;
    ss << "(Router " << this->getId() << ")";
    return ss.str();
}

void Router::handlePacket(Packet *packet){
    Node::handlePacket(packet);
    /*std::cout << "T"
              << SYSTEM_CONTROLLER->getCurrentTime()
              << ":"
              << this->infoString()
              << " is handling "
              << packet->infoString()
              << std::endl;*/
    assert(packet != NULL);

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
        delete H;
        break;
    case Packet::ROUTERROUTE:
        // handing routing table information
        updated = updateRoutingTable(R->getRoutingTable(), R->getLink());
        // If the routing table was updated, broadcast this to all
        // neighbors.
        if (updated) {
            this->broadcastRoutingTable();
        }
        delete R;
        break;
    case Packet::ACK:
        // handle acknowledgement packets: same as data packets
    case Packet::DATA:
        // handle data packets
        (getNextLink(D->getDestination()))->handlePacket(packet);
        break;
    default:
        std::cout << "INVALID PACKET TYPE" << std::endl;
        assert(0);
        exit(0);
        break;
    }
}

void Router::broadcastRoutingTable() {
    /*std::cout << this->infoString() << " starting broadcasting routing table"
        << " at time " << SYSTEM_CONTROLLER->getCurrentTime() << "\n";*/
    for (std::list<Link* >::iterator it = this->links.begin();
            it != this->links.end(); it++)
    {
        RouterRoutingPacket *newRoutingPacket 
            = new RouterRoutingPacket(this, NULL, *it, this->routingTable_p);
        newRoutingPacket->setPreviousNode(this);
        (*it)->handlePacket(newRoutingPacket);
    }
    /*std::cout << this->infoString() << " finished broadcasting routing table"
        << " at time " << SYSTEM_CONTROLLER->getCurrentTime() << "\n";*/
}    

Link* Router::getNextLink(Node *destination) {
    /*std::cout << "getting next link" <<std::endl;*/
	return this->routingTable_p->nextLink(destination);
}

Node* Router::getNextNode(Node *destination) {
    /*std::cout << "getting next node" << std::endl;*/
	Node * result = this->routingTable_p->nextNode(destination);
    if (result == NULL) {
        std::cout << "Can't reach Host "
                  << destination->infoString()
                  << " from Router "
                  << infoString()
                  << ".\n Allow more time for routing table updates "
                  << "or change the network configuration.\n";
        exit(1);
    }
    return result;
}

void Router::update(RoutingTable *t, double linkWgt, bool &changed,
        Node *r, Link *l) {
    (*routingTable_p)[r].first = t->mapping[r].first + linkWgt;
    (*routingTable_p)[r].second = l;
    changed = true;
}


/*  Updates the routing table of this router based on a neighbor's table   */
bool Router::updateRoutingTable(RoutingTable *t, Link *l) {
    assert(t != NULL);
    assert(l != NULL);
    if (this->nodeId == 1 && SYSTEM_CONTROLLER->getCurrentTime() == 10) {
        int x = 1;
    }
    bool changed = false;
    for (std::map<Node*, std::pair<double, Link*> >::iterator it = t->mapping.begin();
        it != t->mapping.end(); ++it) {
        Node *r = it->first;
        if (r != this) {
            // Get the link weight, which is measured in ms
            double linkWgt = l->getDelay() + 
                ((((double)l->getOccupancy() / (double)l->getRate()) * 
                        (double)1000));
            bool noEntry = (routingTable_p->mapping.count(r) == 0);
            if (noEntry) {
                update(t, linkWgt, changed, r, l);
            }
            else {
                bool largerWeight = ((*routingTable_p)[r].first > 
                    linkWgt + t->mapping[r].first + UPDATE_TOLERANCE);
                bool smallerWeight = ((*routingTable_p)[r].first +
                        UPDATE_TOLERANCE < linkWgt + t->mapping[r].first);
                bool wouldNormallyPassL = (routingTable_p->nextLink(r) == l);
                bool nextLinkNotThisLink = (t->nextLink(r) != l);
                bool approxEqual = abs((*routingTable_p)[r].first -
                    linkWgt + t->mapping[r].first) <= UPDATE_TOLERANCE;
                // doesn't seem to do much
                bool lowerId = l->getId() < (*routingTable_p)[r].second->getId();
                if ((smallerWeight && wouldNormallyPassL) || 
                    (largerWeight) ||
                    (approxEqual && lowerId)) {
                    update(t, linkWgt, changed, r, l);
                }
            }
        }
    }

    //std::cout << infoString() << std::endl;
    //debugRoutingTable();
    return changed;
}


void Router::updateSingleNode(Host *host, Link *link) {
    // The router is directly connected to this host through the
    // link. Just set the distance to this host to be the link's
    // delay.
    std::pair<double, Link*> pair = std::make_pair(
            link->getDelay(), link);
    (*this->routingTable_p)[host] = pair;
}

// Prints the routing table of this router to the terminal
void Router::debugRoutingTable() {
for (std::map<Node*, std::pair<double, Link* > >::iterator it
            = this->routingTable_p->mapping.begin();
        it != this->routingTable_p->mapping.end();
        it++)
    {
        Node *node = it->first;
        int id = node->getId();
        int distance = it->second.first;
        Link *link = it->second.second;
        std::cout << "The distance to " << node->infoString() << " is " << distance
            << "via ";
        if (link == 0){
            std::cout << " no idea" << std::endl;
        }
        else std::cout << link->infoString() << std::endl;
    }
}

void Router::print() {
    std::cout << infoString() << " routing table:\n";
    routingTable_p->print();
}
