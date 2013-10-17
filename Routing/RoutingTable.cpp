
#include "RoutingTable.h"


Link* RoutingTable::nextLink(Router *r)
{
	if (r == NULL)
		throw "Router is null";
	return mapping[r];
}

Router* RoutingTable::nextRouter(Router *r)
{
	Link *l = this->nextLink(r);
	if (l->getEnd1() == r) {
		return l->getEnd2();
	}
	return l->getEnd1();
}
