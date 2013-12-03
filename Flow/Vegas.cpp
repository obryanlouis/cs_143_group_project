#include "Vegas.h"
#include "Packet.h"

extern Controller *SYSTEM_CONTROLLER;
const double START_RTT = 10000;
const double TIMEOUT_CONST = 10;
const double FAST_WINDOW_PERIOD = 1000;

const double ALPHA = 1;
const double BETA = 3;

void fastWindowReset(void *arg);

Vegas::Vegas(Flow *f)
    : inRecovery(false)
    , rttmin(DBL_MAX)
    , SLOW_START(f)
{
    outstanding = 1;
    roundTripTime = DBL_MAX;
    fastWindowReset((void *)this);
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

void fastWindowReset(void *arg) {
    Vegas *v = (Vegas *)arg; 
    // w = base rtt * window size / rtt + ALPHA
    v->windowSize = v->rttmin * v->windowSize / v->roundTripTime + ALPHA;

    Event *e = new Event(SYSTEM_CONTROLLER->getCurrentTime() + FAST_WINDOW_PERIOD, 
            &fastWindowReset, (void *)v);
    SYSTEM_CONTROLLER->add(e);
}

void Vegas::ackRecieved(AckPacket *p) {
    std::cout << "Flow progress: " << flow->getProgress() 
        << " out of " << flow->getTotalPackets() <<  std::endl;
    std::cout << "sendNext: " << sendNext << std::endl;

    /** DEBUG **/
    if (sendNext < 0) {
        std::cout << "sendNext < 0: abort\n";
        exit(1);
    }
    if (windowSize < 1) {
        std::cout << "windowsize < 1\n";
        exit(1);
    }
    flow->checkAllRecieved(lastAckRecieved);
    //flow->printRemainingPacketIds();

    /**********/
   
    // Don't consider packets that have been dropped before this
    if (p->getStartTime() < this->lastDroppedTime) return;


    int id = p->getAckId(); 

    outstanding--;
    SLOW_START::updateTimeout(p->getStartTime());
    assert(roundTripTime > 0);

    // set the rttmin
    if (rttmin > roundTripTime)
        rttmin = roundTripTime;

    // slow start
    if (this->windowSize < this->ssthreash) {
        lastAckRecieved = id;
        std::cout << "\t in slow start \n";
        this->windowSize += 1;
        sendAvailablePackets();
    }
    // congestion avoidance
    else {
        std::cout << "\t in congestion avoidance \n";
        // for every ack
        if (lastAckRecieved != id) {
            // if recovery is ending, set the window size to be ssthreash
            if (inRecovery) {
                inRecovery = false;
                windowSize = ssthreash;
            }
            lastAckRecieved = id;
            duplicates = 0;
            double min = windowSize / rttmin;
            std::cout << "min: " << min << std::endl;
            double notmin = windowSize / roundTripTime;
            double difference = min - notmin;
            if (difference < ALPHA) {
                std::cout << "\t\t\t\twindowSize++ in CA: " << windowSize << "\n";
                windowSize++;
            }
            if (difference > BETA) {
                std::cout << "\t\t\t\twindowSize-- in CA: " << windowSize << "\n";
                windowSize--;
            }
            sendAvailablePackets();
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
        }
    }

}

