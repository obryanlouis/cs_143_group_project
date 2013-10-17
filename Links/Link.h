

#ifndef LINK_H
#define LINK_H

#include "Router.h"

class Router;

class Link {
	
private:
	Router *end1;
	Router *end2;
	int	traffic;	// change this
	int length;	// can change this


public:
	int getTraffic() { return this->traffic; }
	void resetTraffic() { this->traffic = 0; }
	int getLength() { return this->length; }
	Router *getEnd1() { return this->end1; }
	Router *getEnd2() { return this->end2; }

};

#endif