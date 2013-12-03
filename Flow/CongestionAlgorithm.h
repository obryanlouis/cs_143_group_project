
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

    virtual void scheduleFirstPacket(double startTime) = 0;
 
    virtual void ackRecieved(AckPacket *packet) = 0;
    virtual AckPacket* makeAckPacket(DataPacket *p) = 0;
    // when DataPacket recieved at destination host, determines
    // what AckPacket should be sent. 

};

class SLOW_START : public CongestionAlgorithm{
private:
    double roundTripTime;
    double timeDeviation;
    double timeout;
    double ssthreash;
    double alpha; // weighting for RTT updates
    double outstanding; 
    int sendNext;

    int lastAckRecieved;
    int duplicates;
    int maxAck;
    double lastDroppedTime;

public: 
    SLOW_START(Flow *in_flow);

    double getTimeOut();

    void scheduleFirstPacket(double startTime);
    virtual void packetDropped(int id);
    virtual void ackRecieved(AckPacket *p);
    void sendPacket(int id, double startTime);
    AckPacket *makeAckPacket(DataPacket *p);
};

#endif
