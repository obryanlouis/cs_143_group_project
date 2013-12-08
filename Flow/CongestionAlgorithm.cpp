
// CongestionAlgorithm.cpp

#include "CongestionAlgorithm.h"
#include "Control.h" // include this here since header doesn't need
#include "Packet.h"

const double START_RTT = 10000;
const double DEFAULT_ALPHA = 0.75;
const double TIMEOUT_CONST = 100;
const int SEND_LIMIT = 10; 

    CongestionAlgorithm::CongestionAlgorithm(Flow *in_flow)
    :cwnd(1.0)
     ,flow(in_flow)
{}

Flow *CongestionAlgorithm::getFlow(){
    return this->flow;
}

double CongestionAlgorithm::getWindowSize(){
    return this->cwnd;
}

AckPacket *CongestionAlgorithm::makeAckPacket(DataPacket *p){
    // make sure the AckPacket is of the right ID
    int id = this->flow->getNextUnrecieved();
    std::cout << "making ackpacket of id" << id << std::endl;
    return new AckPacket(p, id);
}

    SLOW_START::SLOW_START(Flow *in_flow)
    : CongestionAlgorithm(in_flow)
    , ssthresh(DBL_MAX)
    , roundTripTime(START_RTT)
    , timeout(500)
    , alpha(DEFAULT_ALPHA)
    , sendNext(1)
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
    bool wasDropped;
    responsibleCA->packetDropped(id, wasDropped);

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

    double checkAt = startTime 
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
    free(theseArgs);
}

void CongestionAlgorithm::scheduleFirstPacket(double startTime){
    // make event to make first packet
    void (*fp)(void*) = &SLOW_START_sendFirstPacket;
    void **args = (void **)malloc(2* sizeof (void *));
    args[0] = (void*) this;
    args[1] = (void*) 0;

    Event *e = new Event(startTime, fp, (void *) args);
    SYSTEM_CONTROLLER->add(e);
}


void SLOW_START::packetDropped(int id, bool &wasDropped){
    wasDropped = false;
    std::cout << "In SLOW_START::packetDropped()" << std::endl;
    // see if packet not actually dropped
    std::cout << "\t Packet " 
        << id << "has timeout "<< flow->getPacketTime(id) << std::endl;
    if (flow->getPacketTime(id) > SYSTEM_CONTROLLER->getCurrentTime()){
        std::cout << "\t Packet not actually dropped" << std::endl;
        return;
    }

    if (flow->getPacketTime(id) == 0 ) {
        std::cout << "\t Packet got reset by previous timeout " << std::endl;
        return;
    }

    std::cout << "\t Packet DID get dropped" << std::endl;
    wasDropped = true;
    // otherwise, update ssthresh and resend packets
    this->ssthresh = this->cwnd / 2;
    this->cwnd = 1;

    this->flow->resetPackets(id);

    this->sendPacket(flow->nextHostPacket(), SYSTEM_CONTROLLER->getCurrentTime());

    this->outstanding = 1;
    this->sendNext = flow->nextHostPacket() + 1; 
    std::cout << "\t Sendnext at " << sendNext << "\n";

    this->lastDroppedTime = SYSTEM_CONTROLLER->getCurrentTime();

}

void SLOW_START::updateTimeout(double startTime){
    double rtt = SYSTEM_CONTROLLER->getCurrentTime() - startTime;
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

    this->timeout = roundTripTime + 4*timeDeviation + TIMEOUT_CONST; 
}

void SLOW_START::ackRecieved(AckPacket *p){
    std::cout << "In SLOW_START::ackRecieved " << std::endl;
    if (p->getStartTime() < this->lastDroppedTime) return;

    this->updateTimeout(p->getStartTime());


    if (this->cwnd < this->ssthresh) {
        std::cout << "\t in slow start \n";
        this->cwnd += 1;
    }
    else {
        std::cout << "\t in linear growth \n";
        this->cwnd += 1/this->cwnd;
    }

    this->outstanding--;

    int id = p->getAckId();

    // send new packets
    int start = std::max(this->sendNext, id); 
    int i = start;
    for (i; 
            i < start + this->cwnd - this->outstanding
            && i < flow->getTotalPackets(); 
            i++){
        this->sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                + (i- start) *0.0001);
    }

    this->outstanding += i - start;
    std::cout << "\tPackets outstanding" << outstanding << "window size" << cwnd << "threashold" << ssthresh <<  std::endl;

    this->sendNext = i;


    std::cout << "\tCA's next packet will be" << sendNext << std::endl;



}


    TCP_TAHOE::TCP_TAHOE(Flow* in_flow)
    : SLOW_START(in_flow)
    , lastAckRecieved(-1)
      , duplicates(0)
{ 
}


void TCP_TAHOE::ackRecieved(AckPacket *p){
    // Fast retransmit
    std::cout << "In TCP_TAHOE::ackRecieved" << std::endl;
    std::cout << "\tthreashold" << ssthresh << "/n";
    if (p->getStartTime() < lastDroppedTime) {
        return;
    }

    SLOW_START::updateTimeout(p->getStartTime());

    int id = p->getAckId();

    if (id == lastAckRecieved && this->ssthresh < this-> outstanding){
        duplicates++;
        std::cout << "\t duplicate of id " << id << std::endl;
        if (duplicates == 3) {
            std::cout << "\tDuplicates == 3 \n";
            this->ssthresh = outstanding/ 2;
            this->cwnd = 1;
            SLOW_START::sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
            this->outstanding = 1;
            this->sendNext = id + 1;
            // ignore all packets before new packet gets back
            lastDroppedTime = SYSTEM_CONTROLLER->getCurrentTime();
            this->flow->resetPackets(id);
            lastAckRecieved = -1;
            return;
        }
    }
    else {
        lastAckRecieved = id;
        duplicates = 0;
    }



    if (this->cwnd < this->ssthresh) {
        std::cout << "\t in slow start \n";
        this->cwnd += 1;
    }
    else {
        std::cout << "\t in linear growth \n";
        this->cwnd += 1/this->cwnd;
    }

    this->outstanding--;


    // send new packets
    int start = std::max(this->sendNext, id); 
    int i = start;
    for (i; 
            i < start + this->cwnd - this->outstanding
            && i < flow->getTotalPackets() 
            && i < start + SEND_LIMIT;
            i++){
        this->sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                + (i- start) *0.0001);
    }

    this->outstanding += i - start;
    std::cout << "\tPackets outstanding" << outstanding << "window size" << cwnd <<  std::endl;

    this->sendNext = i;


    std::cout << "\tCA's next packet will be" << sendNext << std::endl;

}

void TCP_TAHOE::packetDropped(int id, bool &wasDropped){
    SLOW_START::packetDropped(id, wasDropped);
}

/************************* TCP RENO *********************************/

    TCP_RENO::TCP_RENO(Flow* in_flow)
    : TCP_TAHOE(in_flow)
      , inRecovery(false)
{
}


void TCP_RENO::packetDropped(int id, bool &wasDropped){
    TCP_TAHOE::packetDropped(id, wasDropped);
    if (wasDropped) {
        inRecovery = false;
        lastAckRecieved = -1;
        duplicates = 0;
    }
}

void TCP_RENO::ackRecieved(AckPacket *p){
    // Fast recovery
    std::cout << "In TCP_RENO::ackRecieved" << std::endl;
    std::cout << "\t threashold" << ssthresh << "\n";    
    std::cout << "\tcurrent time " << SYSTEM_CONTROLLER->getCurrentTime() << " packet creation " << p->getStartTime() << "last dropped" << lastDroppedTime << "\n";
    std::cout << "\t Outstanding = " << this->outstanding << std::endl;
    std::cout << "\t Window Size = " << cwnd << std::endl;
    std::cout << "\t sendNext = " << sendNext << std::endl;

    int id = p->getAckId();
    /*
       if (p->getAckId() > sendNext) {
       sendNext = p->getAckId();
       if (outstanding - cwnd <= 1) {
       std::cout << "\t in that weird ass fringe case \n";
       SLOW_START::sendPacket(p->getAckId(), 0);

       }
       }
       */
    if (p->getStartTime() < lastDroppedTime) {
        return;
    }

    if (inRecovery && lastAckRecieved == p->getAckId()) {
        std::cout << "\t currently in recovery \n";
        cwnd++;

        int start = std::max(this->sendNext, id); 
        int i = start;
        for (i; 
                i < start + this->cwnd - this->outstanding
                && i < flow->getTotalPackets(); 
                i++){
            this->sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                    + (i- start) *0.0001);
        }

        this->outstanding += i - start;
        std::cout << "\tPackets outstanding" << outstanding << "window size" << cwnd <<  std::endl;

        this->sendNext = i;
        return; // makes code run for twice as long, but eh. 

    }



    if (id == lastAckRecieved && this->ssthresh < this-> outstanding){
        outstanding--; 
        duplicates++;
        SLOW_START::updateTimeout(p->getStartTime());
        std::cout << "\t duplicate of id " << id << std::endl;
        if (duplicates == 3) {
            std::cout << "\tDuplicates == 3 \n";
            this->ssthresh = outstanding/ 2;
            this->cwnd = ssthresh + 3;
            SLOW_START::sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
            this->outstanding++ ;
            this->sendNext = id + 1; 

            this->inRecovery = true;
            return;
        }

    }
    else {
        lastAckRecieved = id;
        duplicates = 0;
        if (inRecovery) {
            inRecovery = false;
            cwnd = ssthresh;
        }
    }

    this->updateTimeout(p->getStartTime());


    if (this->cwnd < this->ssthresh) {
        std::cout << "\t in slow start \n";
        this->cwnd += 1;
    }
    else {
        std::cout << "\t in linear growth \n";
        this->cwnd += 1/this->cwnd;
    }

    this->outstanding--;

    // send new packets
    int start = std::max(this->sendNext, id); 
    int i = start;
    for (i; 
            i < start + this->cwnd - this->outstanding
            && i < flow->getTotalPackets(); 
            i++){
        this->sendPacket(i, SYSTEM_CONTROLLER->getCurrentTime()
                + (i- start) *0.0001);
    }

    this->outstanding += i - start;


    this->sendNext = i;


    std::cout << "\tCA's next packet will be" << sendNext << std::endl;


}




void SLOW_START::scheduleFirstPacket(double startTime) {
    CongestionAlgorithm::scheduleFirstPacket(startTime);
}

void TCP_TAHOE::scheduleFirstPacket(double startTime) {
    SLOW_START::scheduleFirstPacket(startTime);
}

void TCP_RENO::scheduleFirstPacket(double startTime) {
    TCP_TAHOE::scheduleFirstPacket(startTime);
}

double CongestionAlgorithm::getDiff() { return 0; }

double SLOW_START::getDiff() { return 0; }

double TCP_RENO::getDiff() { return 0; }

double TCP_TAHOE::getDiff() { return 0; }

double CongestionAlgorithm::numOutstanding() { return 0; }

double SLOW_START::numOutstanding() { return outstanding; }

Dumb::Dumb(Flow *in_flow)
    : SLOW_START(in_flow)
    , sendNext(1)
{
    cwnd = 5.0;
}


void Dumb::packetDropped(int id, bool &wasDropped) {
    if (flow->getPacketTime(id) > SYSTEM_CONTROLLER->getCurrentTime()){
        std::cout << "\t Packet not actually dropped" << std::endl;
        return;
    }

    if (flow->getPacketTime(id) == 0 ) {
        std::cout << "\t Packet got reset by previous timeout " << std::endl;
        return;
    }

    std::cout << "\t Packet DID get dropped" << std::endl;

    this->flow->resetPackets(id);

    sendNext = flow->nextHostPacket();
    for (int i = 0; i < cwnd; i++){
        this->sendPacket(sendNext, SYSTEM_CONTROLLER->getCurrentTime());
        sendNext++;
    }



}

void Dumb::ackRecieved(AckPacket *p) {
    
    SLOW_START::sendPacket(sendNext, SYSTEM_CONTROLLER->getCurrentTime());
    sendNext++;
    

}

void Dumb::scheduleFirstPacket(double startTime){
    // make event to make first packet
    void (*fp)(void*) = &SLOW_START_sendFirstPacket;
    for (int i = 0; i < cwnd; i++){
       void **args = (void **)malloc(2* sizeof (void *));  
        args[0] = (void*) this;
        args[1] = (void*) ((long) i);
 

        Event *e = new Event(startTime, fp, (void *) args);
        SYSTEM_CONTROLLER->add(e);
        }
    sendNext = cwnd;
}


