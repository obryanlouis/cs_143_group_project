

#include "Router.h"

using namespace std;

/*	Walks backwards from u to v on a predecessor map of Routers	*/
Link* walkBackwards(Router *u, Router *v, map<Router *, Router *> predecessor);

Link* Router::getNextLink(Router *destination) {
	return this->routingTable->nextLink(destination);
}

Router* Router::getNextRouter(Router *destination) {
	return this->routingTable->nextRouter(destination);
}


/*
 *	The Bellman Ford algorithm. Updates the source router's routing
 *	table.
 *	
 *	nodes: all routers in the system
 *	edges: all edges in the system
 */
void Router::bellmanFord(list<Router*> nodes, list<Link*> edges)
{
	map<Router*, int> distance;
	map<Router*, Router*> predecessor;

	/*	Initialize the graph	*/
	for (list<Router*>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if (*it == this) {
			distance[*it] = 0;
		}
		else {
			distance[*it] = -1;
		}
		predecessor[*it] = NULL;
	}

	/*	Relax edged repeatedly	*/	
	for (int i = 0; i < nodes.size(); i++) {
		for (list<Link*>::iterator it = edges.begin(); 
			it != edges.end(); ++it) {
				int weight = (*it)->getLength();
				Router* u = (*it)->getEnd1();
				Router* v = (*it)->getEnd2();
				if (distance[u] != -1 &&
					distance[u] + weight < distance[v]) {
						distance[v] = distance[u] + weight;
						predecessor[v] = u;
				}
		}
	}

	/*	Set the Routing table mapping	*/
	RoutingTable *t = this->routingTable;
	for (list<Router*>::iterator it = nodes.begin(); 
		it != nodes.end(); ++it) {
			if (*it != this) {
				(*t)[*it] = *walkBackwards(*it, this, &predecessor);
			}
	}
}

Link* Router::walkBackwards(Router *u, Router *v, map<Router *, Router *> *predecessor)
{
	/*	Loop until the predecessor is the source	*/
	Router *finder = v;
	while ((*predecessor)[finder] != u) {
		finder = (*predecessor)[finder];
	}

	/*	Find the link that corresponds to the connection between these
		routers	*/
	for (list<Link*>::iterator it = u->links.begin(); 
		it != u->links.end(); ++it) {
			if ((*it)->getEnd1() == finder || (*it)->getEnd2() == finder) {
				return *it;
			}
	}

	/*	If the execution gets here, there is no link between the router
		and its predecessor :O	*/
	throw "No Link available";
}