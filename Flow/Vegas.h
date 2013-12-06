#ifndef VEGAS_H
#define VEGAS_H

#include "CongestionAlgorithm.h"
#include <cmath>
#include <list>

class Vegas : public SLOW_START {
    private:
        int lastAckRecieved;
        int duplicates;

        bool inRecovery;
        double rttmin;
        void sendAvailablePackets();
        std::list<double> rtts;

    public:
        void maintain();
        void ackRecieved(AckPacket *p);
        void packetDropped(int id);
        Vegas(Flow *f);
 
};

#endif
