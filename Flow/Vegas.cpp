#include "Vegas.h"
#include "Packet.h"

extern Controller *SYSTEM_CONTROLLER;
const double START_RTT = 10000;
const double TIMEOUT_CONST = 10;
const double FAST_WINDOW_PERIOD = 1000;

const double ALPHA = 1;
const double BETA = 3;
const double GAMMA = 1;

void maintainVegas(void *arg);

    Vegas::Vegas(Flow *f)
    : rttmin(DBL_MAX)
      , TCP_RENO(f)
{
    maxincr = 2;
    incr = 0;
    status = 0;
    mode = SLOWSTART; 
    cwnd = 1;
    outstanding = 0;
    packetNumberAfterRetransmission = 3;
    rttcurrent = DBL_MAX;
    rttSetFirstTime = false;
}

void Vegas::scheduleFirstPacket(double startTime) {
    Event *e = new Event(flow->getStartTime(), &maintainVegas, (void *) this);
    SYSTEM_CONTROLLER->add(e);
}

void maintainVegas(void *arg) {
    ((Vegas*)arg)->maintain();
}

void Vegas::slowStart() {
    // A modification of vegas called "gallop vegas"
    // see http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=6182775
    double maxincr = cwnd;
    double diff = cwnd * (1 / rttmin - 1 / rttcurrent);
    if (diff > GAMMA) {
        if (diff >= BETA) {
            cwnd -= (lastincr + diff - BETA);
            ssthresh = 2;
            if (cwnd < 2)
                cwnd = 2;
            // Enter congestion avoidance
            status = 2;
            mode = CONGESTIONAVOIDANCE;
        }
        else {
            if (status == 0) {
                incr = (int)(incr / 2);
                if (incr <= 1) {
                    ssthresh = 2;
                    incr = 1;
                }
                status = 1;
            }
            else {
                status = 0;
            }
            cwnd += incr;
        }
    }
    else {
        cwnd += incr;
        if (incr < maxincr)
            incr += 1;
        status = 0;
    }
}

void Vegas::congestionAvoidance() {
    if (status != 2) {
        // do the motion of original vegas
    }
    else {
        status = 0;
    }
}

// an actual timeout
void Vegas::retransmissionTimeout(int id) {
    maxincr = 2;
    incr = 0;
    status = 0;
    cwnd = 1;
    outstanding = 0;
    mode = SLOWSTART; 
    packetNumberAfterRetransmission = 0;
    sendNext = id;
}

void Vegas::maintain() {
    if (mode == SLOWSTART)
        slowStart();
    else if (mode = CONGESTIONAVOIDANCE)
        congestionAvoidance();

    sendAvailablePackets();
    // the first time this executes, rttcurrent is DBL_MAX
    if (rttSetFirstTime) {
        Event *e = new Event(SYSTEM_CONTROLLER->getCurrentTime() + rttcurrent,
                &maintainVegas, (void *)this);
        SYSTEM_CONTROLLER->add(e);
    }
}

void Vegas::sendAvailablePackets() {
    if (sendNext < lastAckRecieved) {
        std::cout << "Shouldn't need to send this packet\n";
        exit(1);
    }
    int limit = sendNext + cwnd - outstanding;
    // if there are packets left to send
    if (limit > sendNext) {
        for (int i = sendNext; i < limit; i++) {
            SLOW_START::sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                    + (double)(i - sendNext) / 1000);
            outstanding++;
        }
        sendNext = limit;
    }
}

void Vegas::conditionalRetransmit(int id) {
    if (rttcurrent > timeout) {
        // the number of outstanding packets doesn't change
        sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
    }
}

void Vegas::ackRecieved(AckPacket *p) {
    if (p->getStartTime() < lastDroppedTime) {
        return;
    }
    // if in a fast recovery cycle, let reno finish
    if (inRecovery) {
        TCP_RENO::ackRecieved(p);
        return;
    }

    int id = p->getAckId();
    rttcurrent = SYSTEM_CONTROLLER->getCurrentTime() - p->getStartTime();

    if (!rttSetFirstTime) {
        rttSetFirstTime = true;
        maintain();
    }

    if (rttcurrent < rttmin)
        rttmin = rttcurrent;

    if (mode == CONGESTIONAVOIDANCE) { 
        // normal vegas case
        if (id != lastAckRecieved) {
            packetNumberAfterRetransmission++;
            outstanding--;
            duplicates = 0;
            if (packetNumberAfterRetransmission <= 2) {
                conditionalRetransmit(id);
            }
            double diff = cwnd * (1 / rttmin - 1 / rttcurrent);
            if (diff < ALPHA)
                cwnd++;
            else if (diff > BETA)
                cwnd--;
        }
        else {
            // if the difference between the current time and the timestamp
            // recorded is larger than the timeout value, then Vegas retransmits
            // the packet
            // otherwise, Vegas does the same thing as reno
            if (rttcurrent > timeout) {
                conditionalRetransmit(id);
            }
            else {
                if (duplicates >= 3) {
                    packetNumberAfterRetransmission = 0;
                }
                TCP_RENO::ackRecieved(p);
            }
        }
    }

    lastAckRecieved = id;
}

// checks for a timeout to my understanding
void Vegas::packetDropped(int id, bool &wasDropped) {
    TCP_RENO::packetDropped(id, wasDropped);
    if (wasDropped) {
        retransmissionTimeout(id);
    }
}










