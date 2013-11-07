// Node.h

#ifndef NODE_H
#define NODE_H

#include <list>
#include <cstdarg>
#include <iostream>


class Link;
class Packet;

/* Nodes are an abstraction for the links. */
class Node {
private:
    friend class Router;
    friend class Host;
    int nodeId;
    std::list<Link*> links;

public:
    Node();
    Node(int in_id);
    virtual ~Node();

    virtual void handlePacket(Packet* packet) = 0;
    void addLink(Link *link);
};

#endif

