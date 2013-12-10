#include "Vegas.h"
#include "Packet.h"

extern Controller *SYSTEM_CONTROLLER;
const double START_RTT = 10000;
const double TIMEOUT_CONST = 10;
const double FAST_WINDOW_PERIOD = 1000;

const double GAMMA = 0.3;
const double ALPHA = 0.5;
const double BETA  = 0.55;

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
    rttEstimate = 0;
}

void firstVegasPacket(void *arg) {
    Vegas *v = (Vegas *)arg;
    v->sendPacket(0, SYSTEM_CONTROLLER->getCurrentTime());
}

void Vegas::scheduleFirstPacket(double startTime) {
    Event *e = new Event(startTime, &firstVegasPacket, (void *) this);
    SYSTEM_CONTROLLER->add(e);
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

void Vegas::sendAvailablePackets() {
    if (sendNext < lastAckReceived) {
        sendNext = lastAckReceived;
    }

    // Send packets up to the minimum of 
    // 1. The number of packets within the window minus the outstanding
    // 2. The number of packets that can fit into the link buffer at the
    //    current host.
    int limit = sendNext + cwnd - outstanding;
    Link *l = flow->getStart()->getLink();
    double capacity = l->getCapacity();
    Buffer *b = l->getBuffer(flow->getStart());
    double capacityUsed = b->capacityUsed;
    limit = std::min(limit, 
            (int)((0.5) * (capacity - capacityUsed)) / Packet::DATASIZE + sendNext);
    if (limit != sendNext + cwnd - outstanding) {
        std::cout << "";
    }
    // if there are packets left to send
    if (limit > sendNext) {
        std::cout << "Sending " << limit - sendNext 
            << " packets "<< std::endl;
        for (int i = sendNext; i < limit; i++) {
            SLOW_START::sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                    + (double)(i - sendNext));
            outstanding++;
        }
        sendNext = limit;
    }
}

void Vegas::ackReceived(AckPacket *p) {

    if (p->getStartTime() < lastDroppedTime) {
        return;
    }

    outstanding--;
    if (outstanding < 0)
        outstanding = 0;

    int id = p->getAckId();
    rttcurrent = SYSTEM_CONTROLLER->getCurrentTime() - p->getStartTime();

    if (rttcurrent + 0.00001 < rttmin) {
        rttmin = rttcurrent;
    }

    if (mode == SLOWSTART) {
        double gamma = 0.95;
        if (getDiff() > GAMMA) {
            mode = CONGESTIONAVOIDANCE;
        }
        else {
            if (parity == 0) {
                cwnd += 1;
                sendAvailablePackets();
            }
            else {
                parity = 1;
            }
        }
    }


    if (mode == CONGESTIONAVOIDANCE) { 
        // normal vegas case
        if (id != lastAckReceived) {
            // if in recovery previously, not in recovery anymore
            // set cwnd to ssthresh
            if (inRecovery) {
                inRecovery = false;
                cwnd = ssthresh;
                mode = SLOWSTART;
                rttEstimate = 0;
                return;
            }

            packetNumberAfterRetransmission++;
            duplicates = 0;
            if (packetNumberAfterRetransmission <= 2) {
                if (rttcurrent > timeout) {
                    sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
                    outstanding++;
                }
            }
            std::cout << "rttmin: " << rttmin << "\n";
            std::cout << "rttEstimate: " << rttEstimate << "\n";
            std::cout << "rttcurrent: " << rttcurrent << "\n";
            double diff = getDiff();
            std::cout << "Diff in Vegas CA: " << diff << std::endl;
            double inc = ((double) 1) / (cwnd);
            if (diff < ALPHA) {
                cwnd += inc;
                std::cout << "Incrementing window size\n";
            }
            else if (diff > BETA) {
                cwnd -= inc;
                std::cout << "decrementing window size\n";
            }
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

                    packetNumberAfterRetransmission = 0;
                    this->ssthresh = outstanding / 2;
                    this->cwnd = ssthresh + 3;
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
        sendAvailablePackets();
    }

    lastAckReceived = id;
}

void Vegas::packetDropped(int id, bool &wasDropped) {
    double tempLastDroppedTime = lastDroppedTime;

    TCP_TAHOE::packetDropped(id, wasDropped);
    if (wasDropped) {
        inRecovery = false;
        lastAckReceived = -1;
        duplicates = 0;
    }

    // If there was actually a timeout
    if (wasDropped && 
            (tempLastDroppedTime + rttcurrent < SYSTEM_CONTROLLER->getCurrentTime())) {
        retransmissionTimeout(id);
    }
}


double Vegas::getDiff() {
    // If rttcurrent hasn't been set for the first time, return a placeholder
    // value. This occurs when graphing the parameter.
    if (rttcurrent == DBL_MAX)
        return 0;
    double gamma = 0.5;
    if (rttEstimate == 0)
        rttEstimate = rttcurrent;
    rttEstimate = gamma * rttEstimate + (1 - gamma) * rttcurrent;
    return cwnd / rttmin - cwnd / rttEstimate;
}





