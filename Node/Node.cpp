
#include "Node.h"

Node::Node() {}

Node::Node(int in_id, std::list<Link*> l)
    : nodeId(in_id)
    , links(l)
{}

Node::~Node()
{}

