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
    windowSize = 2;
    outstanding = 0;
    roundTripTime = DBL_MAX;
    maintain(); 
}

void maintainVegas(void *arg) {
    ((Vegas*)arg)->maintain();
}

void Vegas::maintain() {
    sendAvailablePackets();
    double avgrtt = 0;
    if (rtts.size() != 0) {
        for (std::list<double>::iterator it = rtts.begin(); it != rtts.end();
                it++)
        {
            avgrtt += *it;
        }
        avgrtt /= rtts.size();
        rtts.clear();
    }
    else {
        avgrtt = 150;
    }
    
    Event *e = new Event(SYSTEM_CONTROLLER->getCurrentTime() + avgrtt,
        &maintainVegas, (void *)this);
    SYSTEM_CONTROLLER->add(e);
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
    roundTripTime = SYSTEM_CONTROLLER->getCurrentTime() - p->getStartTime();

    // set the rttmin
    if (rttmin > roundTripTime)
        rttmin = roundTripTime;

    rtts.push_back(roundTripTime);

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
            windowSize += 1 ;
        }
        if (difference > BETA) {
            std::cout << "\t\t\t\twindowSize-- in CA: " << windowSize << "\n";
            windowSize -= 1 ;
        }
        if (windowSize < 1)
            windowSize /= 1;
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



