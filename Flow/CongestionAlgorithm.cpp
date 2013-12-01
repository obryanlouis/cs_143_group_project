
// CongestionAlgorithm.cpp

#include "CongestionAlgorithm.h"
#include "Control.h" // include this here since header doesn't need
#include "Packet.h"

const double START_RTT = 10000;
const double DEFAULT_ALPHA = 0.75;

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

TCP_RENO::TCP_RENO(Flow *in_flow)
    : CongestionAlgorithm(in_flow)
    , ssthreash(DBL_MAX)
    , roundTripTime(START_RTT)
    , timeout(START_RTT)
    , alpha(DEFAULT_ALPHA)
    , sendNext(1)
    , lastAckRecieved(-1)
    , duplicates(0)
{
}

double TCP_RENO::getTimeOut(){
    return this->timeout;
}

void TCP_RENO_SeeIfPacketDropped(void *arg){
    void **theseArgs = (void **) arg;
    TCP_RENO *responsibleCA = (TCP_RENO *) theseArgs[0];
    int id = (int) ((long) theseArgs[1]);

    // have congestion algorithm check if packet was dropped
    responsibleCA->packetDropped(id);
    
    // clean up
    free(theseArgs);
}


void TCP_RENO_sendPacket(void *arg){
    void **theseArgs = (void **) arg;
    TCP_RENO *responsible = (TCP_RENO *) theseArgs[0];
    int id = (int) ((long) theseArgs[1]);

    std::cout << "Making Packet of id " << id << std::endl;

    double checkAt = SYSTEM_CONTROLLER->getCurrentTime() 
                            + responsible->getTimeOut();

    // make packet and send to flow to put into system
    DataPacket *p = new DataPacket (id, responsible->getFlow(), \
                            SYSTEM_CONTROLLER->getCurrentTime());
    responsible->getFlow()->sendNewPacket(p, checkAt);
    
    // make timeout event 
    void (*fp)(void*) = &TCP_RENO_SeeIfPacketDropped;
    Event *e = new Event (checkAt, fp, theseArgs);

    // no cleanup needed since still using args
}

void TCP_RENO::scheduleFirstPacket(double startTime){
    // make event to make first packet
    void (*fp)(void*) = &TCP_RENO_sendPacket;
    void **args = (void **)malloc(2* sizeof (void *));
    args[0] = (void*) this;
    args[1] = (void*) 0;

    Event *e = new Event(startTime, fp, (void *) args);
    SYSTEM_CONTROLLER->add(e);
}


void TCP_RENO::packetDropped(int id){
    // see if packet not actually dropped
    if (flow->getPacketTime(id) > SYSTEM_CONTROLLER->getCurrentTime()){
        return;
    }
    // otherwise, update ssthreash and resend packets
    this->ssthreash = this->windowSize / 2;
    this->windowSize = 1;
    
    int i = id; 
    for (i; 
            i < id + this->windowSize && i < flow->getTotalPackets(); 
            ++i){
        void (*fp)(void*) = &TCP_RENO_sendPacket;
        void **args = (void **)malloc(2* sizeof (void *));
        args[0] = (void*) this;
        args[1] = (void*) ((long) i);

        Event *e = new Event(SYSTEM_CONTROLLER->getCurrentTime(),
                fp, (void *) args);
        SYSTEM_CONTROLLER->add(e);
    }

    this->outstanding = i - id - 1; // might need to change.
    this->sendNext = i; 



}

void TCP_RENO::ackRecieved(AckPacket *p){

    // update RTT
    double rtt = SYSTEM_CONTROLLER->getCurrentTime() - p->getStartTime();
    if (this->roundTripTime = START_RTT){
        this->roundTripTime = rtt;
        this->timeDeviation = rtt;
    }
    else {
        this->roundTripTime *= (1-alpha);
        this->roundTripTime += alpha * rtt;

        this->timeDeviation = (1-alpha) * this->timeDeviation + alpha * \
                            abs(rtt - this->roundTripTime);
    }

    this->timeout = roundTripTime + 4 * timeDeviation; 

    int id = p->getId();

    // see if ack is duplicate. If so, update system and send packet.
    if (lastAckRecieved == id) {
        duplicates++;
        if (duplicates == 3) {   
            this->windowSize /= 2;
            int i = id; 
            for (i; 
                    i < id + this->windowSize && i < flow->getTotalPackets(); 
                    ++i){
                void (*fp)(void*) = &TCP_RENO_sendPacket;
                void **args = (void **)malloc(2* sizeof (void *));
                args[0] = (void*) this;
                args[1] = (void*) ((long)i);

                Event *e = new Event(SYSTEM_CONTROLLER->getCurrentTime(),
                                    fp, (void *) args);
                SYSTEM_CONTROLLER->add(e);
            }

            this->outstanding = i - id - 1; // might need to change.
            this->sendNext = i; 
            return; // retransmitted packets, so break out early
        }
            
    }        

    else {
        lastAckRecieved = id;
        duplicates = 0;
    }

    this->outstanding--;
    if (this->outstanding < 0) this->outstanding = 0;
    
    // see if in ssthreash
    if (this->ssthreash < this->windowSize){
        this->windowSize += 1 / this->windowSize;
    }
    else {
        this->windowSize += 1;
    }
    
    // send new packets
    int i = this->sendNext; 
    for (i; 
            i < this->sendNext + this->windowSize - this->outstanding
            && i < flow->getTotalPackets(); 
            i++){
    std::cout << "scheduling new packet of id" << i << std::endl;
        void (*fp)(void*) = &TCP_RENO_sendPacket;
        void **args = (void **)malloc(2* sizeof (void *));
        args[0] = (void*) this;
        args[1] = (void*) ((long) i);

        Event *e = new Event(SYSTEM_CONTROLLER->getCurrentTime(),
                            fp, (void *) args);
        SYSTEM_CONTROLLER->add(e);
    }

    std::cout << "CA's next packet will be" << i << std::endl;

    this->outstanding = i - this->sendNext - 1;
    std::cout << "Packets outstanding" << outstanding << "window size" << windowSize << std::endl;
    
    this->sendNext = i;


}


AckPacket *TCP_RENO::makeAckPacket(DataPacket *p){
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
