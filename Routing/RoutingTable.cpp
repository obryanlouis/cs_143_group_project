// RoutingTable.cpp

#include "RoutingTable.h"

RoutingTable::RoutingTable()
{
    std::cout << &(this->mapping) << std::endl;
}

RoutingTable::RoutingTable(RoutingTable *old)
    :mapping(old->mapping)
{   
    /*std::cout << "old: " << &(old->mapping) << "size=" << old->mapping.size()<< \
     "| new: " << &mapping<<std::endl << "size=" << mapping.size() << std::endl;*/
}

RoutingTable::~RoutingTable() { }

Link* RoutingTable::nextLink(Node *r)
{
	if (r == NULL)
		throw "Router is null";
    if (mapping.count(r) == 0) {
        return NULL;
    }
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

void RoutingTable::print() {
    for (std::map<Node*, std::pair<double, Link*> >::iterator it = mapping.begin();
            it != mapping.end(); it++)
    {
        if (it->first != NULL && it->second.second != NULL) {
            std::cout << "Distance to Node " << it->first->infoString()
                << " is " << it->second.first << " via " 
                << it->second.second->infoString()
                << "\n";
        }
    }
}

