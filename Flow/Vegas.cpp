#include "Vegas.h"
#include "Packet.h"

extern Controller *SYSTEM_CONTROLLER;
const double START_RTT = 10000;
const double TIMEOUT_CONST = 10;
const double FAST_WINDOW_PERIOD = 1000;

const double ALPHA = 0.2;
const double BETA = 0.25;

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
    parity = 0;
    outstanding = 0;
    packetNumberAfterRetransmission = 3;
    rttcurrent = DBL_MAX;
    rttSetFirstTime = false;
    //GAMMA = SYSTEM_CONTROLLER->caRate(this);
    GAMMA = ALPHA;
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
    double actualRate = cwnd * (1 / rttcurrent);
    if (actualRate > GAMMA) {
        mode = CONGESTIONAVOIDANCE;
        status = 2;
        /*if (diff >= BETA) {
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
        }*/
    }
    else {
        if (parity == 0) {
            cwnd += incr;
            if (incr < maxincr)
                incr += 1;
            status = 0;
        }
        else {
            parity = 1;
        }
    }
}

void Vegas::congestionAvoidance() {
    if (status != 2) {
        // do the motion of original vegas
    }
    // gallop vegas resets status to 0
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
    else if (mode == CONGESTIONAVOIDANCE)
        congestionAvoidance();

    if (!rttSetFirstTime) {
        sendAvailablePackets();
    }

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

    // DEBUG
    std::cout << "Number of outstanding packets: " << outstanding << "\n";
    std::cout << "Number of packets in system: " 
        << SYSTEM_CONTROLLER->numberOfPacketsInSystem() << "\n";



    int limit = sendNext + cwnd - outstanding;
    // if there are packets left to send
    if (limit > sendNext) {
        std::cout << "Sending " << limit - sendNext 
            << " packets "<< std::endl;
        if (limit - sendNext > 63) {
            std::cout << "sending too many!\n";
            exit(1);
        }
        for (int i = sendNext; i < limit; i++) {
            SLOW_START::sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                    + (double)(i - sendNext) / 1000);
            outstanding++;
        }
        sendNext = limit;
    }
}

/*void sendVegasCallback(void * arg) {
    Vegas *v = (Vegas *) arg;
    v->sendAvailablePackets();

    Event *e = new Event(SYSTEM_CONTROLLER->getCurrentTime() + 2,
        &sendVegasCallback, arg);
    SYSTEM_CONTROLLER->add(e);
}*/

void Vegas::ackRecieved(AckPacket *p) {
    outstanding--;
    if (outstanding < 0)
        outstanding = 0;

    if (p->getStartTime() < lastDroppedTime) {
        return;
    }

    int id = p->getAckId();
    rttcurrent = SYSTEM_CONTROLLER->getCurrentTime() - p->getStartTime();

    if (rttcurrent + 0.00001 < rttmin) {
        std::cout << "rttmin has become rttcurrent at time: " <<
            SYSTEM_CONTROLLER->getCurrentTime() << std::endl;
        rttmin = rttcurrent;
    }

    if (!rttSetFirstTime) {
        rttSetFirstTime = true;
        maintain();
        //sendVegasCallback((void *)this);
    }

    if (mode == CONGESTIONAVOIDANCE) { 
        // normal vegas case
        if (id != lastAckRecieved) {
            // if in recovery previously, not in recovery anymore
            // set cwnd to ssthresh
            if (inRecovery) {
                inRecovery = false;
                cwnd = ssthresh;
            }

            packetNumberAfterRetransmission++;
            duplicates = 0;
            if (packetNumberAfterRetransmission <= 2) {
                if (rttcurrent > timeout) {
                    sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
                    outstanding++;
                }
            }
            double diff = getDiff();
            std::cout << "Diff in Vegas CA: " << diff << std::endl;
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
                sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
            }
            else {
                duplicates++;
                if (duplicates == 3) {

                    // DEBUG
                    std::cout << "duplicates == 3\n";
                    std::cout << "cwnd: " << cwnd << std::endl;
                    std::cout << "outstanding: " << outstanding << std::endl;
                    std::cout << "diff: " << getDiff()
                        << std::endl;
                    std::cout << "rttcurrent: " << rttcurrent << std::endl;
                    SYSTEM_CONTROLLER->makeDebugPlots(); 
                    exit(1);

                    packetNumberAfterRetransmission = 0;
                    this->ssthresh = outstanding / 2;
                    this->cwnd = ssthresh + 3;
                    this->outstanding = cwnd;
                    SLOW_START::sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
                    this->outstanding++;
                    this->sendNext = id + 1; 
                    this->inRecovery = true;
                }

                else if (duplicates > 3) {
                    cwnd++;
                }
            }
        }
    }

    lastAckRecieved = id;
    sendAvailablePackets();
}

// checks for a timeout to my understanding
void Vegas::packetDropped(int id, bool &wasDropped) {
    double tempLastDroppedTime = lastDroppedTime;
    TCP_RENO::packetDropped(id, wasDropped);
    if (wasDropped && 
            (tempLastDroppedTime + rttcurrent < SYSTEM_CONTROLLER->getCurrentTime())) {
        retransmissionTimeout(id);
    }
}


double Vegas::getDiff() {
    return cwnd * (1 / rttmin - 1 / rttcurrent);
}
