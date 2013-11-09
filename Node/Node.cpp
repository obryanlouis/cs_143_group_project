
#include "Node.h"
#include "Packet.h"

Node::Node() {}

Node::Node(int in_id)
    : nodeId(in_id)
{}

Node::~Node()
{}

void Node::handlePacket(Packet *packet) {
    // DEBUG
    if (packet == NULL) {
        std::cout << "Node::handlePacket : Packet is null\n";
    }
    packet->setPreviousNode(this);
}

void Node::addLink(Link *link) {
    this->links.push_back(link);
}

int Node::getId() {
    return this->nodeId;
}
