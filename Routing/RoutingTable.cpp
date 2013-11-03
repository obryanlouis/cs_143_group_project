
#include "RoutingTable.h"


Link* RoutingTable::nextLink(Node *r)
{
	if (r == NULL)
		throw "Router is null";
	return mapping[r].second;
}

Node* RoutingTable::nextNode(Node *r)
{
	Link *l = this->nextLink(r);
	if (l->getEnd1() == r) {
		return l->getEnd2();
	}
	return l->getEnd1();
}
