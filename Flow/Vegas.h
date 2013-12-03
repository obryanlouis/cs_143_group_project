#ifndef VEGAS_H
#define VEGAS_H

#include "CongestionAlgorithm.h"
#include <cmath>

class Vegas : public TCP_RENO {
    private:
        bool inRecovery;
        double rttmin;
        void sendAvailablePackets();
    public:
        void ackRecieved(AckPacket *p);
        Vegas(Flow *f);
 
};

#endif
