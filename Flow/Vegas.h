#ifndef VEGAS_H
#define VEGAS_H

#include "CongestionAlgorithm.h"
#include <cmath>
#include <list>


class Vegas : public TCP_RENO {
    private:
        enum Mode {
            SLOWSTART,
            CONGESTIONAVOIDANCE
        };
        Mode mode;
        int status;
        int parity;
        double incr;
        double maxincr;
        double lastincr;
            // The last increment of cwnd
        int packetNumberAfterRetransmission;
            // used for the first two packets after retransmission

        double rttmin;
        double rttcurrent;
        bool rttSetFirstTime;
        void sendAvailablePackets();

    public:
        void maintain();
        void ackRecieved(AckPacket *p);
        void packetDropped(int id, bool &wasDropped);
        void congestionAvoidance();
        void retransmissionTimeout(int id);
        void slowStart();
        void scheduleFirstPacket(double startTime);
        Vegas(Flow *f);
        double getDiff();
 
};

#endif
