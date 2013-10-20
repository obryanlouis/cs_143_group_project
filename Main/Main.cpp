

#include "Link.h"
#include "Router.h"
#include "RoutingTable.h"

using namespace std;

int main( int argc, const char* argv[] ) {
	Router r1, r2;
    RoutingTable t;
    Link l;

    l.setEnds(&r1, &r2);

	return 0;
}
