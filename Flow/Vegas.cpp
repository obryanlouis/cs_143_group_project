#include "Vegas.h"
#include "Packet.h"

extern Controller *SYSTEM_CONTROLLER;
const double START_RTT = 10000;
const double TIMEOUT_CONST = 10;

const double ALPHA = 10;
const double BETA = 13;

Vegas::Vegas(Flow *f)
    : inRecovery(false)
    , rttmin(DBL_MAX)
    , TCP_RENO(f)
{}

void Vegas::sendAvailablePackets() {
    int limit = sendNext + windowSize - outstanding;
    for (int i = sendNext; i < limit; i++) {
        SLOW_START::sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                + (double)(i - sendNext) / 1000);
        outstanding++;
    }
    sendNext = limit;
}

void Vegas::ackRecieved(AckPacket *p) {
   
    // Don't consider packets that have been dropped before this
    if (p->getStartTime() < this->lastDroppedTime) return;


    int id = p->getAckId(); 
    lastAckRecieved = std::max(id, lastAckRecieved);

    // slow start
    if (this->windowSize < this->ssthreash) {
        std::cout << "\t in slow start \n";
        SLOW_START::ackRecieved(p);
        // set the rttmin; slow_start::ackrecieved calls updatetimeout
        if (rttmin > roundTripTime)
            rttmin = roundTripTime;
    }
    // congestion avoidance
    else {
        outstanding--;
        SLOW_START::updateTimeout(p->getStartTime());
        // set the rttmin
        if (rttmin > roundTripTime)
            rttmin = roundTripTime;
        std::cout << "\t in congestion avoidance \n";
        // for every ack
        if (lastAckRecieved != id) {
            if (inRecovery) {
                inRecovery = false;
                windowSize = ssthreash;
            }
            duplicates = 0;
            double min = windowSize / rttmin;
            std::cout << "min: " << min << std::endl;
            double notmin = windowSize / roundTripTime;
            double difference = min - notmin;
            if (difference < ALPHA) {
                windowSize++;
            }
            if (difference > BETA) {
                windowSize--;
            }
        }
        // for every loss
        else {
            duplicates++;
            std::cout << "duplicates: " << duplicates << std::endl;
            if (duplicates == 3) {
                inRecovery = true;
                windowSize = std::max((double)1, windowSize / 2);
                ssthreash = windowSize; 

                SLOW_START::sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
            }
            else if (duplicates > 3) {
                windowSize++;
                sendAvailablePackets();
            }
            return;
        }
    }

}

