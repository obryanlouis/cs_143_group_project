
#include "RoutingTable.h"


Link* RoutingTable::nextLink(Router *r)
{
	if (r == NULL)
		throw "Router is null";
	return mapping[r].second;
}

Router* RoutingTable::nextRouter(Router *r)
{
	Link *l = this->nextLink(r);
	if (l->getEnd1() == r) {
		return l->getEnd2();
	}
	return l->getEnd1();
}
