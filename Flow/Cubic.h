#ifndef CUBIC_H
#define CUBIC_H

#include "CongestionAlgorithm.h"
#include <cmath>

class Cubic : public SLOW_START {
    private:
        friend void maintainCubic(void *arg);
        bool tcp_friendliness;
        bool fast_convergence;
        double W_last_max;
        double epoch_start;
        double origin_point;
        double dMin;
        double W_tcp;
        double K;
        double ack_cnt;
        double cnt;
        double cwnd_cnt;
        
        int sendNext;
        int outstanding;
        double lastReset;
        int lastAck;
        int duplicates;
        
        void cubic_reset();
        void cubic_update();
        void cubic_tcp_friendliness();
  
        void packet_loss();
        void sendAvailablePackets();
    
    public:
        virtual void ackRecieved(AckPacket *p);
        virtual void packetDropped(int id, bool &wasDropped);
        Cubic(Flow *f);
 
};

#endif
