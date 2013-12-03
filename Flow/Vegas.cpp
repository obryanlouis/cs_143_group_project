#include "Vegas.h"
#include "Packet.h"

extern Controller *SYSTEM_CONTROLLER;
const double START_RTT = 10000;
const double TIMEOUT_CONST = 10;
const double FAST_WINDOW_PERIOD = 1000;

const double ALPHA = 1;
const double BETA = 3;

Vegas::Vegas(Flow *f)
    : inRecovery(false)
    , rttmin(DBL_MAX)
    , SLOW_START(f)
{
    outstanding = 1;
    roundTripTime = DBL_MAX;
    //fastWindowReset((void *)this);
}

void Vegas::sendAvailablePackets() {
    int limit = sendNext + windowSize - outstanding;
    // if there are packets left to send...
    if (limit > sendNext) {
        for (int i = sendNext; i < limit; i++) {
            SLOW_START::sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                    + (double)(i - sendNext) / 1000);
            outstanding++;
        }
        sendNext = limit;
    }
}

void Vegas::ackRecieved(AckPacket *p) {
  
    // Don't consider packets that have been dropped before this
    if (p->getStartTime() < this->lastDroppedTime) return;


    int id = p->getAckId(); 

    outstanding--;
    SLOW_START::updateTimeout(p->getStartTime());
    assert(roundTripTime > 0);

    // set the rttmin
    if (rttmin > roundTripTime)
        rttmin = roundTripTime;

    // for every ack
    if (lastAckRecieved != id) {
        lastAckRecieved = id;
        duplicates = 0;
        double min = windowSize / rttmin;
        std::cout << "min: " << min << std::endl;
        double notmin = windowSize / roundTripTime;
        double difference = min - notmin;
        if (difference < ALPHA) {
            std::cout << "\t\t\t\twindowSize++ in CA: " << windowSize << "\n";
            windowSize += 1 / roundTripTime;
        }
        if (difference > BETA) {
            std::cout << "\t\t\t\twindowSize-- in CA: " << windowSize << "\n";
            windowSize -= 1 / roundTripTime;
        }
        if (windowSize < 1)
            windowSize = 1;
        sendAvailablePackets();
    }
    
}

void Vegas::packetDropped(int id) {
    std::cout << "Vegas dropped packet\n";
    windowSize /= 2;
    if (windowSize < 1)
        windowSize = 1;
    SLOW_START::sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
    sendNext = id + 1;
}
