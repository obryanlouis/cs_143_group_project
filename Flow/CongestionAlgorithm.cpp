
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
    , lastDupTime(0)
    , numResent(0)
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
    
    this->sendPacket(flow->nextHostPacket(), SYSTEM_CONTROLLER->getCurrentTime());

    this->outstanding = 1;
    this->sendNext = flow->nextHostPacket() + 1; 
 std::cout << "\t Sendnext at " << sendNext << "\n";

    this->duplicates = 0;
    this->lastAckRecieved = -1;

    this->lastDroppedTime = SYSTEM_CONTROLLER->getCurrentTime();

}

void SLOW_START::ackRecieved(AckPacket *p){

    if (p->getStartTime() < this->lastDroppedTime) return;

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








    double packetStart = p->getStartTime();
    double currentTime = SYSTEM_CONTROLLER->getCurrentTime();
    int id = p->getAckId(); 

    if (id == lastAckRecieved) { 
        if (flow->getPacketTime(id) == 0) return;
        duplicates++;
        if (duplicates == 3) {
            outstanding = 0;
            windowSize = windowSize / 2;
            ssthreash = std::max((double)2, windowSize);
            flow->resetPackets(id);
            int limit = id + windowSize;
            for (int i = id; i < limit; i++) {
                sendPacket(i, currentTime);
                outstanding++;
            }
            sendNext = limit;
        }
        else if (duplicates > 3) {
            windowSize++;
            outstanding--;
            int limit = sendNext + windowSize - outstanding;
            for (int i = sendNext; i < limit; i++) {
                outstanding++;
                sendPacket(i, currentTime);
            }
            sendNext = limit;
        } 
        return;
    }

    if (duplicates > 3) {
        windowSize = ssthreash;
    }

    duplicates = 0;
    lastAckRecieved = id;




void SLOW_START::ackRecieved(AckPacket *p){
std::cout << "In SLOW_START::ackRecieved " << std::endl;
   this->updateTimeout(p->getStartTime());
    
    if (this->windowSize < this->ssthreash) {
        std::cout << "\t in slow start \n";
        this->windowSize += 1;
    }
    else {
        std::cout << "\t in linear growth \n";
        this->windowSize += 1/this->windowSize;
    }



    this->outstanding--;
   
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

double SLOW_START::getThresh() {
    return ssthreash;
}

double SLOW_START::getOutstanding() {
    return outstanding;
}

<<<<<<< HEAD
=======
void TCP_TAHOE::ackRecieved(AckPacket *p){
    // Fast retransmit
std::cout << "In TCP_TAHOE::ackRecieved" << std::endl;
    if (p->getStartTime() < lastDroppedTime) {
        return;
    }

    int id = p->getAckId();

    if (id == lastAckRecieved && this->ssthreash < this-> outstanding){
        duplicates++;
        SLOW_START::updateTimeout(p->getStartTime());
std::cout << "\t duplicate of id " << id << std::endl;
        if (duplicates == 3) {
std::cout << "\tDuplicates == 3 \n";
            this->ssthreash = outstanding/ 2;
            this->windowSize = 1;
            lastDroppedTime = SYSTEM_CONTROLLER->getCurrentTime();
            SLOW_START::sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
            this->outstanding = 1;
            this->sendNext = id + 1;
            this->flow->resetPackets(id);
            return;
        }

    }
    else {
        lastAckRecieved = id;
        duplicates = 0;
    }

    SLOW_START::ackRecieved(p);
>>>>>>> 8de42295223a5ac57fe2b68f11cb3a65a2bf6b00

AckPacket *SLOW_START::makeAckPacket(DataPacket *p){
    // make sure the AckPacket is of the right ID
    int id = this->flow->getNextUnrecieved();
    std::cout << "making ackpacket of id" << id << std::endl;
    return new AckPacket(p, id);
}


/************************* TCP RENO *********************************/

TCP_RENO::TCP_RENO(Flow* in_flow)
    : TCP_TAHOE(in_flow)
    , inRecovery(false)
{
}


void TCP_RENO::packetDropped(int id){
    TCP_TAHOE::packetDropped(id);
    inRecovery = false;
    lastAckRecieved = -1;
    duplicates = 0;
}

void TCP_RENO::ackRecieved(AckPacket *p){
    // Fast recovery
std::cout << "In TCP_RENO::ackRecieved" << std::endl;
std::cout << "\t threashold" << ssthreash << "\n";    
std::cout << "\tcurrent time " << SYSTEM_CONTROLLER->getCurrentTime() << " packet creation " << p->getStartTime() << "last dropped" << lastDroppedTime << "\n";
std::cout << "\t Outstanding = " << this->outstanding << std::endl;
std::cout << "\t Window Size = " << windowSize << std::endl;
std::cout << "\t sendNext = " << sendNext << std::endl;
    if (p->getAckId() > sendNext) {
        sendNext = p->getAckId();
        if (outstanding - windowSize <= 1) {
std::cout << "\t in that weird ass fringe case \n";
            SLOW_START::sendPacket(p->getAckId(), 0);
            
        }
    }

    if (p->getStartTime() < lastDroppedTime) {
        return;
    }

    int id = p->getAckId();


    if (id == lastAckRecieved && this->ssthreash < this-> outstanding){
        outstanding--; 
        duplicates++;
        SLOW_START::updateTimeout(p->getStartTime());
std::cout << "\t duplicate of id " << id << std::endl;
        if (duplicates == 3) {
std::cout << "\tDuplicates == 3 \n";
            this->ssthreash = outstanding/ 2;
            this->windowSize = ssthreash + 3;
            SLOW_START::sendPacket(id, SYSTEM_CONTROLLER->getCurrentTime());
            this->outstanding++ ;
        
            this->inRecovery = true;
            return;
        }

        if (inRecovery) {
std::cout << "\t currently in recovery \n";
            windowSize++;

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


        }

    }
    else {
        lastAckRecieved = id;
        duplicates = 0;
        if (inRecovery) {
            inRecovery = false;
            windowSize = ssthreash;
        }
    }

    SLOW_START::ackRecieved(p);


}

