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
        // The last increment of cwnd
        int packetNumberAfterRetransmission;
        // used for the first two packets after retransmission

        double rttmin;
        double rttcurrent;
        bool rttSetFirstTime;
        void sendAvailablePackets();
        double rttEstimate;

        friend void sendVegasCallback(void *arg);

    public:
        void ackReceived(AckPacket *p);
        void packetDropped(int id, bool &wasDropped);
        void retransmissionTimeout(int id);
        void scheduleFirstPacket(double startTime);
        Vegas(Flow *f);
        double getDiff();

};

#endif
