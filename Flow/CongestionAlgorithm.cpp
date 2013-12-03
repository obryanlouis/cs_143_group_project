
// CongestionAlgorithm.cpp

#include "CongestionAlgorithm.h"
#include "Control.h" // include this here since header doesn't need
#include "Packet.h"

const double START_RTT = 10000;
const double DEFAULT_ALPHA = 0.75;
const double TIMEOUT_CONST = 1;


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
    , inRecovery(false)
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


void TCP_RENO::sendPacket(int id, double startTime){

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
    void (*fp)(void*) = &TCP_RENO_SeeIfPacketDropped;
    Event *e = new Event (checkAt, fp, args);
    SYSTEM_CONTROLLER->add(e);
}

void TCP_RENO_sendFirstPacket(void *arg){
    void **theseArgs = (void **) arg;
    TCP_RENO *responsible = (TCP_RENO *) theseArgs[0];
    int id = (int) ((long) theseArgs[1]);

    std::cout << "\tMaking Packet of id " << id << std::endl;

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
    void (*fp)(void*) = &TCP_RENO_sendFirstPacket;
    void **args = (void **)malloc(2* sizeof (void *));
    args[0] = (void*) this;
    args[1] = (void*) 0;

    Event *e = new Event(startTime, fp, (void *) args);
    SYSTEM_CONTROLLER->add(e);
}


void TCP_RENO::packetDropped(int id){
std::cout << "In TCP_RENO::packetDropped()" << std::endl;
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
    this->ssthreash = this->windowSize / 2;
    this->windowSize = 1;

    this->flow->resetPackets(id);
    
    this->sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());

    this->outstanding = 1;
    this->sendNext = id + 1; 

}

void TCP_RENO::ackRecieved(AckPacket *p){
std::cout << "In TCP_RENO::ackRecieved " << std::endl;
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

    //this->timeout = roundTripTime + 4 * timeDeviation + TIMEOUT_CONST; 
    this->timeout = 500;

    int id = p->getAckId();

    // see if ack is duplicate. If so, update system and send packet.
    if (lastAckRecieved == id) {
        if (!inRecovery) {
            inRecovery = true;

            this->ssthreash = std::max((double)2, this->windowSize / 2);
            this->windowSize = 1;

            this->flow->resetPackets(id);
            
            this->sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());

            this->outstanding = 1;
            this->sendNext = id + 1; 
            return;
        }
        else {
            return;
        }
/*
std::cout << "\t Duplicate ACKS recieved of id " << id << std::endl;
        duplicates++;
        if (duplicates == 3) {   
std::cout << "\t Duplicate ACKS = 3 " << std::endl;
            this->ssthreash = this->windowSize / 2;
            this->windowSize = this->windowSize / 2 + 3;
            int i = id; 
            for (i; 
                    i < id + this->windowSize && i < flow->getTotalPackets(); 
                    ++i){
                this->sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime());
            }

            this->outstanding = i - id; // might need to change.
            this->sendNext = i; 
            return; // retransmitted packets, so break out early
        }
        
        else if (duplicates > 3) {
            this->outstanding--;
            this->windowSize += 1;
            int i = this->sendNext;
            for (i; 
                    i < sendNext + this->windowSize - this->outstanding 
                    && i < flow->getTotalPackets(); 
                    ++i){
                this->sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime());
            }

            this->outstanding += i - this->sendNext;
            this->sendNext = i;
            return;

        }*/

    }

    else {
        if (duplicates >= 3) {
            this->windowSize = this->ssthreash;
        }

        lastAckRecieved = id;
        duplicates = 0;
    }

    inRecovery = false;

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
        this->sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime());
    }


    std::cout << "\tCA's next packet will be" << i << std::endl;

    this->outstanding += i - this->sendNext;
    std::cout << "\tPackets outstanding" << outstanding << "window size" << windowSize <<  std::endl;
    
    this->sendNext = i;



}

double TCP_RENO::getThresh() {
    return ssthreash;
}

double TCP_RENO::getOutstanding() {
    return outstanding;
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
