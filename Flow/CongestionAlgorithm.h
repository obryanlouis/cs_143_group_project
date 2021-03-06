
// CongestionAlgorithm.h

#ifndef CongestionAlgorithm_H
#define CongestionAlgorithm_H

#include <cfloat>
#include <cmath>

class AckPacket;
class DataPacket;
class Flow;

enum CongestionAlgorithmType {
    SLOW,
    TAHOE,
    RENO,
    VEGAS,
    CUBIC
};


class CongestionAlgorithm{
    protected:
        double cwnd;
        Flow *flow;
        bool done;

    public:
        CongestionAlgorithm(Flow *in_flow);

        void end();
        Flow* getFlow();
        double getWindowSize();

        virtual void scheduleFirstPacket(double startTime);

        virtual void ackReceived(AckPacket *packet) = 0;
        AckPacket* makeAckPacket(DataPacket *p);
        // when DataPacket received at destination host, determines
        // what AckPacket should be sent. 
        virtual double numOutstanding();

};

class SLOW_START : public CongestionAlgorithm{
    protected:
        double roundTripTime;
        double timeDeviation;
        double timeout;
        double ssthresh;
        double alpha; // weighting for RTT updates
        double outstanding; 
        int sendNext;

        int maxAck;
        double lastDroppedTime;

    public: 
        SLOW_START(Flow *in_flow);

        double getTimeOut();

        virtual void scheduleFirstPacket(double startTime);
        virtual void packetDropped(int id, bool &wasDropped);
        virtual void ackReceived(AckPacket *p);
        void sendPacket(int id, double startTime);
        void updateTimeout(double time);
        AckPacket *makeAckPacket(DataPacket *p);
        virtual double numOutstanding();
};


class TCP_TAHOE : public SLOW_START{
    protected:
        int lastAckReceived;
        int duplicates;

    public:
        TCP_TAHOE(Flow *in_flow);

        virtual void ackReceived(AckPacket *p);
        virtual void packetDropped(int id, bool &wasDropped);
        virtual void scheduleFirstPacket(double startTime);
};

class TCP_RENO : public TCP_TAHOE {
    protected:
        bool inRecovery;

    public:
        TCP_RENO(Flow *in_flow);
        virtual void ackReceived(AckPacket *p);
        virtual void packetDropped(int id, bool &wasDropped);
        virtual void scheduleFirstPacket(double startTime);

};


#endif
