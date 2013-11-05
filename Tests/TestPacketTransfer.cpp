#include "Link.h"
#include "Router.h"
#include "Packet.h"
#include <gtest/gtest.h>

// This file tests that the transfer of packets between links and routers works

using namespace testing;

TEST(TestPacketTransfer, RouterToLink) {
    Router r1;
    Router r2;
    Link l;
    r1.addLink(l);
    r2.addLink(l);
    Host h1, h2;
    Flow f(0, 60, &h1, &h2);
    Packet p(NULL, Packet::DATA, r1, r2, time_t 0, 1024, Flow *f, std::string id);
    
    r1.handlePacket(&p);
}


int main(int argc, char **argv) {
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
