// Flow.cpp

#include "Flow.h"
#include <sstream>
#include <climits>

extern Controller *SYSTEM_CONTROLLER; 

Flow::Flow(int in_ID, int in_size, Host *in_source, Host *in_destination,
        CongestionAlgorithmType algType)
    : flowId(in_ID)
    , size(in_size)
    , source(in_source)
    , destination(in_destination)
    , progress(0)
    , dataSent(0)
    , dataReceived(0)
{
    // Calculate the number of packets we need based on size of flow 
    this->totalPackets = (in_size + (Packet::DATASIZE - 1)) / Packet::DATASIZE;

    if (this->totalPackets == 0) this->totalPackets = 1;

    for (int i = 0; i < totalPackets; i++){
        this->packets[i] = 0;
    }

    for (int i = 0; i < totalPackets; i++){
        this->acks.insert(i);
    }

    in_source->setFlow(this);
    in_destination->setFlow2(this);

    // make congestion algorithm
    switch (algType) {
        case RENO:
            congestionAlgorithm_p = new TCP_RENO(this);
        break;
        case VEGAS:
//            congestionAlgorithm_p = new TCP_Vegas(this);
        break;

        default:
            std::cout << "Invalid routing type" << std::endl;
            exit(1);
    } 
    

}

Flow::~Flow()
{
    delete congestionAlgorithm_p;
}


void Flow::startFlow(double startTime){
    this->congestionAlgorithm_p->scheduleFirstPacket(startTime);
}

void Flow::sendNewPacket(DataPacket *p, double timeOut){
    this->updateDataSent(p->getSize());
    this->packets[p->getId()] = timeOut;
    this->source->handlePacket(p);
}
    
int Flow::getNextUnrecieved(){
    return *(acks.begin());
}


AckPacket* Flow::atDest(DataPacket *p){
    std::cout << "In Flow:atDest " << std::endl;
    // let the flow know that the destination has recieved the data
    this->acks.erase(p->getId());
    std::cout << "\tTried to erase " << p->getId() << " new next unrecieved " << getNextUnrecieved() << std::endl;
    // let the congestion control algorithm make the ack packet
    AckPacket *ack = this->congestionAlgorithm_p->makeAckPacket(p);
 
   return ack;
}

void Flow::handlePacket(AckPacket *p) {
    SYSTEM_CONTROLLER->checkPackets();
    std::cout << "--Flow " << flowId << " has received an acknowledgement"
        << " packet for packet " << p->getId() <<
        "at time " << SYSTEM_CONTROLLER->getCurrentTime() << "\n";
    std::cout << p->getStartTime() << std::endl;

    // see if packet recieved before
    if (this->packets[p->getId()] != DBL_MAX){
        // update progress
        this->progress++;
        // done with that packet ID, so set its timeout to super long
        // so it'll never get sent
        this->packets[p->getId()] = DBL_MAX; 
        std::cout << "changed to dbl_max";
    }  

    // update window size
    congestionAlgorithm_p->ackRecieved(p);

    // update other stats: dataSent, dataReceived
    std::cout << "Flow " << flowId << " progress: " << progress <<
        " out of " << totalPackets << " received\n";
    this->dataReceived += p->getSize();

    delete p;
}


double Flow::getPacketTime(int id){
    return packets[id];
}


           
Host *Flow::getStart(){
    return source;
}

Host *Flow::getDestination(){
    return destination;
}


int Flow::getId() {
    return this->flowId;
}

double Flow::getStats(std::string stat, int period) {
    if (stat.compare("send rate") == 0) {
        return (((double)this->dataSent) / (double)period) *
            ((double) 8/ (double) 1000);
    }
    else if (stat.compare("receive rate") == 0) {
        return (((double)this->dataReceived) / (double)period) *
            ((double) 8/ (double) 1000);
    }
    // TODO:
    else if (stat.compare("rtt") == 0) {
        return (double)this->outstanding * (double)period /
            (double)this->dataReceived;
    }
    else if (stat.compare("window") == 0) {
        return congestionAlgorithm_p->getWindowSize();
    }
}


void Flow::resetStats() {
    dataSent = 0;
    dataReceived = 0;
}

void Flow::updateDataSent(int bytes) {
    dataSent += bytes;
}

void Flow::updateDataReceived(int bytes) {
    dataReceived += bytes;
}

std::string Flow::infoString(){
    std::stringstream ss;
    ss << "(Flow " << this->getId() << ")";
    return ss.str();
}


int Flow::getTotalPackets(){
    return totalPackets;
}
