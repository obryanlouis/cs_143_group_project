
#include "Node.h"

Node::Node() {}

Node::Node(int in_id)
    : nodeId(in_id)
{}

Node::~Node()
{}

void Node::handlePacket(Packet *packet) {
    std::cout << "Node packet handler.\n";
}

void Node::addLink(Link *link) {
    this->links.push_back(link);
}
