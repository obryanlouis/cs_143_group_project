

#ifndef NODE_H
#define NODE_H

#include <list>

#include "Link.h"
#include "Packet.h"

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
  virtual ~Node(){}

  virtual void handlePacket(Packet* packet) = 0;

};

Node::Node() {}

Node::Node(int in_id)
    : nodeId(in_id)
{}


#endif
