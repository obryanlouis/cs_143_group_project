#include "Cubic.h"
#include "Packet.h"
#include "Control.h"

#include <cmath>

extern Controller *SYSTEM_CONTROLLER;
const double START_RTT = 10000;
const double TIMEOUT_CONST = 10;
const double FAST_WINDOW_PERIOD = 10;

const double C = 0.4;
const double BETA = 0.2;


Cubic::Cubic(Flow *f)
    : tcp_friendliness(true)
    , fast_convergence(true)
    , dMin(0)
    , sendNext(1)
    , outstanding(0)
    , lastReset(0)
    , lastAck(-1)
    , duplicates(0)
    , SLOW_START(f)
    , cwnd_cnt(0) // ?
{
    cubic_reset();
    ssthresh = 50;
};




void Cubic::sendAvailablePackets() {
    int limit = sendNext + cwnd - outstanding;

    std::cout << "sending " << limit - sendNext << "\n";    // if there are packets left to send...
    if (limit > sendNext) {
        for (int i = sendNext; i < limit && i < flow->getTotalPackets(); i++) {
            SLOW_START::sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                    + (double)(i - sendNext) / 1000);
            outstanding++;
        }
        sendNext = limit;
    }
}


void maintainCubic (void *arg) {

    std::cout << "in maintain at time " << SYSTEM_CONTROLLER->getCurrentTime() << "\n";
    Cubic *v = (Cubic *) arg;
    v->sendAvailablePackets();
    std::cout << "windowsize is" << v->cwnd << "rtt is" << v->roundTripTime << "outstanding" << v->outstanding; 
    double nextMaintain = SYSTEM_CONTROLLER->getCurrentTime() + v->roundTripTime;
    Event *e = new Event(nextMaintain, &maintainCubic, (void *) arg);
    SYSTEM_CONTROLLER->add(e);


}


void Cubic::ackRecieved(AckPacket *p){
    // if packet was sent a very, very long time ago, ignore it.
    if (p->getStartTime() < lastReset) return;
    if (p->getAckId() == 239) {
        std::cout << "hi";
    }

    std::cout << "ackRecieved handling packet of id " << p->getAckId();

    outstanding--;
    SLOW_START::updateTimeout(p->getStartTime());  
    if (p->getAckId() == 1) maintainCubic(this);

    // check for packet loss via acks. 
    if (p->getAckId() == lastAck) {
        duplicates++;
        if (duplicates == 3) {
            packet_loss();
            outstanding++;
            lastReset = SYSTEM_CONTROLLER->getCurrentTime();
            SLOW_START::sendPacket(p->getAckId(), SYSTEM_CONTROLLER->getCurrentTime());
            return;
        }
    }

    else {
        lastAck = p->getAckId();
        duplicates = 0;
    }

    // TAKEN FROM PAPER
    double oldtime = p->getStartTime();

    if (dMin != 0) dMin = std::min(dMin, oldtime);
    else dMin = oldtime;

    if (cwnd <= ssthresh) cwnd++;
    else {
        // cnt = cubic_update(); <- from paper. cnt changed in cubic_update
        cubic_update();  // Not in paper, but does above. 
        if (cwnd_cnt > cnt) {
           cwnd ++;
           cwnd_cnt = 0;
        }

        else cwnd_cnt++;

    }
    // END TAKEN FROM PAPER


}

void Cubic::packetDropped(int id, bool &wasDropped){
    if (flow->getPacketTime(id) > SYSTEM_CONTROLLER->getCurrentTime()){
        return;
    }
    if (flow->getPacketTime(id) == 0 ) {
    std::cout << "\t other packet got timedout \n";
        return;
    }
    std::cout << "\t Packet DID get dropped" << std::endl;

    flow->resetPackets(id);
    lastReset = SYSTEM_CONTROLLER->getCurrentTime();

    // TAKEN FROM PAPER
    cubic_reset();
    // END TAKEN FROM PAPER

    packet_loss();
    SLOW_START::sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
    outstanding++;
    sendNext = flow->nextHostPacket();
 
}

// TAKEN FROM PAPER
void Cubic::packet_loss(){
   epoch_start = 0;
    if (cwnd < W_last_max and fast_convergence) {
        W_last_max = cwnd * (2-BETA) / 2;
    }
    else W_last_max = cwnd;

    cwnd = cwnd * (1 - BETA);
    ssthresh = cwnd;
}
// END TAKEN FROM PAPER

// TAKEN FROM PAPER
void Cubic::cubic_update() {
    ack_cnt++;
    if (epoch_start <= 0) {
        epoch_start = SYSTEM_CONTROLLER->getCurrentTime();
        if (cwnd < W_last_max) {
            K = pow( (W_last_max - cwnd) / C, 1/3);
            origin_point = W_last_max;
        }
        else {
            K = 0;
            origin_point = cwnd;
        }

        ack_cnt = 1;
        W_tcp = cwnd;
    }
    double t = SYSTEM_CONTROLLER->getCurrentTime() + dMin - epoch_start;
    double target = origin_point + pow(C*(t-K),3);
    if (target > cwnd) cnt = cwnd / (target - cwnd);
    else cnt = 100 * cwnd;

    if (tcp_friendliness) cubic_tcp_friendliness();
}
// END TAKEN FROM PAPER

// TAKEN FROM PAPER
void Cubic::cubic_tcp_friendliness() {
    W_tcp = W_tcp + 3*BETA/(2-BETA) * ack_cnt/cwnd;
    ack_cnt = 0;
    if (W_tcp > cwnd) {
        double max_cnt = cwnd / (W_tcp - cwnd);
        if (cnt > max_cnt) cnt = max_cnt;
    }
}
// END TAKEN FROM PAPER

// TAKEN FROM PAPER
void Cubic::cubic_reset() {
    W_last_max = 0;
    epoch_start = 0;
    origin_point = 0;
    dMin = 0;
    W_tcp = 0;
    K = 0;
    ack_cnt = 0;
}
// END TAKEN FROM PAPER
