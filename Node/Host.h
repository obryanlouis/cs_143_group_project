

#ifndef HOST_H
#define HOST_H

#include "Link.h"
#include "Flow.h"

class Host {
private:
  int ID;
  Link *link;
  Flow *flow;

public:
  Host(int in_ID, Link *in_link, Flow *in_flow);
  ~Host(){}

  Link* getLink();
  Flow* getFlow();
  handlePack

};

#endif
