
#include "RoutingTable.h"

RoutingTable::RoutingTable()
    :mapping()
{
    std::cout << &(this->mapping) << std::endl;
}

// Copy-constructor
RoutingTable::RoutingTable(RoutingTable *old)
    :mapping(old->mapping)
{   
    std::cout << "old: " << &(old->mapping) << "size=" << old->mapping.size()<< \
     "| new: " << &mapping<<std::endl << "size=" << mapping.size() << std::endl;
}

RoutingTable::~RoutingTable() { }

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

bool RoutingTable::containsEntry(Node *node) {
    return this->mapping.count(node);
}

