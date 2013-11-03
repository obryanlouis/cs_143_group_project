

#ifndef NODE_H
#define NODE_H

#include <list>

#include "Link.h"
#include "Packet.h"
#include "RoutingTable.h"

class Link;
class Packet;

/* Nodes are an abstraction for the links. */
class Node {
private:
  friend class Router;
  friend class Host;
  int ID;
  std::list<Link*> links;

public:
  Node(){}
  Node(int in_id, std::list<Link*> l):
    ID(in_id),
    links(l) {}
  virtual ~Node(){}


  virtual void handlePacket(Packet* packet) = 0;

};


#endif
