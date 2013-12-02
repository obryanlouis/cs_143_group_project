
// CongestionAlgorithm.h

#ifndef CongestionAlgorithm_H
#define CongestionAlgorithm_H

#include <cfloat>
#include <cmath>

class AckPacket;
class DataPacket;
class Flow;

enum CongestionAlgorithmType {
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

class TCP_RENO : public CongestionAlgorithm{
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

    bool inRecovery;
    void recovery(int id);

public: 
    TCP_RENO(Flow *in_flow);

    double getTimeOut();

    void scheduleFirstPacket(double startTime);
    void packetDropped(int id);
    void ackRecieved(AckPacket *p);
    void sendPacket(int id, double startTime);
    AckPacket *makeAckPacket(DataPacket *p);
    void incrementPacketsSent();
};

/*
class TCP_Vegas : public CongestionAlgorithm{
public:
    TCP_Vegas(Flow *in_flow);

    void scheduleFirstPacket(double startTime);
 
    void packetDropped(int id);
    void ackRecieved(AckPacket *p);
    void sendMorePackets();
    AckPacket *makeAckPacket(DataPacket *p);

};
*/ 
#endif
