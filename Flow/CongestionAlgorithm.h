
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
    VEGAS
};


class CongestionAlgorithm{
protected:
    double windowSize;
    Flow *flow;

public:
    CongestionAlgorithm(Flow *in_flow);

    Flow* getFlow();
    double getWindowSize();

    void scheduleFirstPacket(double startTime);
 
    virtual void ackRecieved(AckPacket *packet) = 0;
    AckPacket* makeAckPacket(DataPacket *p);
    // when DataPacket recieved at destination host, determines
    // what AckPacket should be sent. 

};

class SLOW_START : public CongestionAlgorithm{
protected:
    double roundTripTime;
    double timeDeviation;
    double timeout;
    double ssthreash;
    double alpha; // weighting for RTT updates
    double outstanding; 
    int sendNext;

    int maxAck;
    double lastDroppedTime;

public: 
    SLOW_START(Flow *in_flow);

    double getTimeOut();

    void scheduleFirstPacket(double startTime);
    virtual void packetDropped(int id);
    virtual void ackRecieved(AckPacket *p);
    void sendPacket(int id, double startTime);
    void updateTimeout(double time);
    AckPacket *makeAckPacket(DataPacket *p);
};


class TCP_TAHOE : public SLOW_START{
protected:
    int lastAckRecieved;
    int duplicates;

public:
    TCP_TAHOE(Flow *in_flow);
    
    virtual void ackRecieved(AckPacket *p);
    virtual void packetDropped(int id);
};


class TCP_RENO : public TCP_TAHOE {
private:
    bool inRecovery;

public:
    TCP_RENO(Flow *in_flow);
    virtual void ackRecieved(AckPacket *p);
    virtual void packetDropped(int id);

};


#endif
