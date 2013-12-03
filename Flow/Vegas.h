#ifndef VEGAS_H
#define VEGAS_H

#include "CongestionAlgorithm.h"
#include <cmath>

class Vegas : public SLOW_START {
    private:
        int lastAckRecieved;
        int duplicates;

        bool inRecovery;
        double rttmin;
        void sendAvailablePackets();
    public:
        void ackRecieved(AckPacket *p);
        Vegas(Flow *f);
 
};

#endif
