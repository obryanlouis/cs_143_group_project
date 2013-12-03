
// CongestionAlgorithm.cpp

#include "CongestionAlgorithm.h"
#include "Control.h" // include this here since header doesn't need
#include "Packet.h"

const double START_RTT = 10000;
const double DEFAULT_ALPHA = 0.75;
const double TIMEOUT_CONST = 10;


CongestionAlgorithm::CongestionAlgorithm(Flow *in_flow)
    :windowSize(1.0)
    ,flow(in_flow)
    {}

Flow *CongestionAlgorithm::getFlow(){
    return this->flow;
}

double CongestionAlgorithm::getWindowSize(){
    return this->windowSize;
}

SLOW_START::SLOW_START(Flow *in_flow)
    : CongestionAlgorithm(in_flow)
    , ssthreash(DBL_MAX)
    , roundTripTime(START_RTT)
    , timeout(START_RTT)
    , alpha(DEFAULT_ALPHA)
    , sendNext(1)
    , lastAckRecieved(-1)
    , duplicates(0)
    , maxAck(0)
    , lastDroppedTime(0)
{
}

double SLOW_START::getTimeOut(){
    return this->timeout;
}

void SLOW_START_SeeIfPacketDropped(void *arg){
    void **theseArgs = (void **) arg;
    SLOW_START *responsibleCA = (SLOW_START *) theseArgs[0];
    int id = (int) ((long) theseArgs[1]);

    // have congestion algorithm check if packet was dropped
    responsibleCA->packetDropped(id);
    
    // clean up
    free(theseArgs);
}


void SLOW_START::sendPacket(int id, double startTime){

    if (id < this->maxAck - 1) {
        std::cout << "shouldn't need to send this id " << std::endl;
        std::cout << "maxack, id = " << maxAck << "," << id << std::endl;
        exit(1);
    }

    std::cout << "\tMaking Packet of id " << id << std::endl;
    void **args = (void **)malloc(2* sizeof (void *));
    args[0] = (void*) this;
    args[1] = (void*) ((long)id);

    double checkAt = SYSTEM_CONTROLLER->getCurrentTime() 
                            + this->getTimeOut();

    // make packet and send to flow to put into system
    DataPacket *p = new DataPacket (id, this->getFlow(), \
                            SYSTEM_CONTROLLER->getCurrentTime());
    this->getFlow()->sendNewPacket(p, checkAt);
    
    // make timeout event 
    void (*fp)(void*) = &SLOW_START_SeeIfPacketDropped;
    Event *e = new Event (checkAt, fp, args);
    SYSTEM_CONTROLLER->add(e);
}

void SLOW_START_sendFirstPacket(void *arg){
    void **theseArgs = (void **) arg;
    SLOW_START *responsible = (SLOW_START *) theseArgs[0];
    int id = (int) ((long) theseArgs[1]);

    std::cout << "\tMaking Packet of id " << id << std::endl;

    double checkAt = SYSTEM_CONTROLLER->getCurrentTime() 
                            + responsible->getTimeOut();

    // make packet and send to flow to put into system
    DataPacket *p = new DataPacket (id, responsible->getFlow(), \
                            SYSTEM_CONTROLLER->getCurrentTime());
    responsible->getFlow()->sendNewPacket(p, checkAt);
    
    // make timeout event 
    void (*fp)(void*) = &SLOW_START_SeeIfPacketDropped;
    Event *e = new Event (checkAt, fp, theseArgs);

    // no cleanup needed since still using args
}

void SLOW_START::scheduleFirstPacket(double startTime){
    // make event to make first packet
    void (*fp)(void*) = &SLOW_START_sendFirstPacket;
    void **args = (void **)malloc(2* sizeof (void *));
    args[0] = (void*) this;
    args[1] = (void*) 0;

    Event *e = new Event(startTime, fp, (void *) args);
    SYSTEM_CONTROLLER->add(e);
}


void SLOW_START::packetDropped(int id){
std::cout << "In SLOW_START::packetDropped()" << std::endl;
    // see if packet not actually dropped
std::cout << "\t Packet " << id << "has timeout "<< flow->getPacketTime(id) << std::endl;
    if (flow->getPacketTime(id) > SYSTEM_CONTROLLER->getCurrentTime()){
std::cout << "\t Packet not actually dropped" << std::endl;
        return;
    }
    
    if (flow->getPacketTime(id) == 0) {
std::cout << "\t Packet got reset by previous timeout " << std::endl;
        return;
    }

std::cout << "\t Packet DID get dropped" << std::endl;
    // otherwise, update ssthreash and resend packets
    this->ssthreash = this->outstanding / 2;
    this->windowSize = 1;

    this->flow->resetPackets(id);
    
    this->sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());

    this->outstanding = 1;
    this->sendNext = flow->getNextUnrecieved(); 

    this->duplicates = 0;
    this->lastAckRecieved = -1;

    this->lastDroppedTime = SYSTEM_CONTROLLER->getCurrentTime();

}

void SLOW_START::ackRecieved(AckPacket *p){
std::cout << "In SLOW_START::ackRecieved " << std::endl;
std::cout << "\t Outstanding = " << this->outstanding << std::endl;
    // update RTT
    double rtt = SYSTEM_CONTROLLER->getCurrentTime() - p->getStartTime();
    if (this->roundTripTime = START_RTT){
        this->roundTripTime = rtt;
        this->timeDeviation = rtt;
    }
    else {
        this->roundTripTime *= (1-alpha);
        this->roundTripTime += alpha * rtt;

        this->timeDeviation = (1-alpha) * this->timeDeviation + alpha * 
                            abs(rtt - this->roundTripTime);
    }

    this->timeout = roundTripTime + timeDeviation + TIMEOUT_CONST; 


    if (this->windowSize < this->ssthreash) {
        std::cout << "\t in slow start \n";
        this->windowSize += 1;
    }
    else {
        std::cout << "\t in linear growth \n";
        this->windowSize += 1/this->windowSize;
    }

    if (p->getStartTime() < this->lastDroppedTime) return;

    this->outstanding--;
   
    int id = p->getAckId();

    // send new packets
    int start = std::max(this->sendNext, id); 
    int i = start;
    for (i; 
            i < start + this->windowSize - this->outstanding
            && i < flow->getTotalPackets(); 
            i++){
        this->sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                            + (i- start) *0.0001);
    }

    this->outstanding += i - start;
    std::cout << "\tPackets outstanding" << outstanding << "window size" << windowSize <<  std::endl;
    
    this->sendNext = i;


    std::cout << "\tCA's next packet will be" << sendNext << std::endl;



}


AckPacket *SLOW_START::makeAckPacket(DataPacket *p){
    // make sure the AckPacket is of the right ID
    int id = this->flow->getNextUnrecieved();
    std::cout << "making ackpacket of id" << id << std::endl;
    return new AckPacket(p, id);
}




/*
TCP_Vegas::TCP_Vegas(Flow* in_flow)
    : CongestionAlgorithm(in_flow)
{
}


void TCP_Vegas::packetDropped(){


}

void TCP_Vegas::ackRecieved(AckPacket *p){

}

AckPacket *TCP_Vegas::makeAckPacket(DataPacket *p){

}
*/
