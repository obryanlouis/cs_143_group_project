// Flow.cpp

#include "Flow.h"

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
    , totalPacketDelay(0)
    , packetsReceived(0)
    , totalLinkLoss(0)
      , done(false)
{
    // Calculate the number of packets we need based on size of flow 
    this->totalPackets = (in_size + (Packet::DATASIZE - 1)) / Packet::DATASIZE;

    if (this->totalPackets == 0) this->totalPackets = 1;

    for (int i = 0; i < totalPackets; i++){
        this->packets[i] = 0;
    }

    for (int i = 1; i <= totalPackets; i++){
        this->acks.insert(i);
    }

    in_source->setFlow(this);
    in_destination->setFlow2(this);

    // make congestion algorithm
    // TODO:
    switch (algType) {
        case SLOW:
            congestionAlgorithm_p = new SLOW_START(this);
            break;
        case TAHOE:
            congestionAlgorithm_p = new TCP_TAHOE(this);
            break;
        case RENO:
            congestionAlgorithm_p = new TCP_RENO(this);
            break;
        case VEGAS:
            congestionAlgorithm_p = new Vegas(this);
            break;
        case CUBIC:
            congestionAlgorithm_p = new Cubic(this);
            break;
        case DUMB:
            congestionAlgorithm_p = new Dumb(this);
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
    this->start = startTime;
    this->congestionAlgorithm_p->scheduleFirstPacket(startTime);
}

double Flow::getStartTime() {
    return start;
}

void Flow::sendNewPacket(DataPacket *p, double timeOut){
    if (!done) {
        this->updateDataSent(p->getSize());
        this->packets[p->getId()] = timeOut;
        this->source->handlePacket(p);
    }
}

int Flow::getNextUnrecieved(){
    return *(acks.begin());
}

int Flow::nextHostPacket(){
    for (int i = 0; i < totalPackets; i++){
        if (packets[i] != DBL_MAX) return i;
    }
    return -1;
}

AckPacket* Flow::atDest(DataPacket *p){
    std::cout << "In Flow:atDest " << std::endl;
    // let the flow know that the destination has recieved the data
    if (*(acks.begin()) == p->getId()) this->acks.erase(p->getId());
    /*std::cout << "\tTried to erase " << p->getId() << " new next unrecieved " << getNextUnrecieved() << std::endl;*/
    // let the congestion control algorithm make the ack packet
    AckPacket *ack = this->congestionAlgorithm_p->makeAckPacket(p);

    // update packet delay stats: totalPacketDelay, packetsReceived
    this->totalPacketDelay +=
        (SYSTEM_CONTROLLER->getCurrentTime() - p->getStartTime());
    this->packetsReceived++;
    totalPacketsRecieved++;

    return ack;
}

void Flow::handlePacket(AckPacket *p) {
    if (done)
        return;

    SYSTEM_CONTROLLER->checkPackets();
    std::cout << "--Flow " << flowId << " has received an acknowledgement"
        << " packet for packet " << p->getAckId() <<
        "at time " << SYSTEM_CONTROLLER->getCurrentTime() << "\n";
    std::cout << p->getStartTime() << std::endl;

    int id = p->getAckId();
    this->progress = id;

    id--;

    for (id; id >= 0; id--){
        this->packets[id] = DBL_MAX;
    }

    double rtt = SYSTEM_CONTROLLER->getCurrentTime() - p->getStartTime();
    updateRTT(rtt);


    /*
    // see if packet recieved before
    if (this->packets[p->getId()] != DBL_MAX){
    // update progress
    this->progress++;
    // done with that packet ID, so set its timeout to super long
    // so it'll never get sent
    this->packets[p->getId()] = DBL_MAX; 
    std::cout << "changed to dbl_max";
    }  
    */

    // update window size
    congestionAlgorithm_p->ackRecieved(p);

    // update other stats: dataSent, dataReceived
    std::cout << "Flow " << flowId << " progress: " << progress <<
        " out of " << totalPackets << " received\n";
    this->dataReceived += p->getSize();

    if (progress == totalPackets) {
        end();
    }


    delete p;
}

void Flow::resetPackets(int id){

    for (id = 0; id < this->totalPackets; id++){
        if (this->packets[id] != DBL_MAX) this->packets[id] = 0;
    }

}


double Flow::getPacketTime(int id){
    std::map<int, double>::iterator it = packets.find(id);
    if (it == packets.end()) {
        std::cout << "Packet id not stored\n";
        exit(1);
    }
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
    if (done)
        return 0.00000001;
    if (stat.compare("send rate") == 0) {
        return (((double)this->dataSent) / (double)period) *
            ((double) 8/ (double) 1000);
    }
    else if (stat.compare("receive rate") == 0) {
        return (((double)this->dataReceived) / (double)period) *
            ((double) 8/ (double) 1000);
    }
    else if (stat.compare("delay") == 0) {
        return totalPacketDelay / packetsReceived;
    }
    else if (stat.compare("window") == 0) {
        return congestionAlgorithm_p->getWindowSize();
    }
    else if (stat.compare("rtt") == 0) {
        return getRTT();
    }
    else if (stat.compare("reno") == 0) {
        return SYSTEM_CONTROLLER->getThroughput() * getRTT() *
            std::sqrt(SYSTEM_CONTROLLER->getLinkLoss(totalLinkLoss)
                    / totalPacketsRecieved) -1.22;
    }
    else if (stat.compare("vegas") == 0) {
        return congestionAlgorithm_p->getDiff();
    }
    else if (stat.compare("outstanding packets") == 0) {
        return congestionAlgorithm_p->numOutstanding();
    }
    /*else if (stat.compare("thresh") == 0) {
      return congestionAlgorithm_p->getThresh();
      }
      else if (stat.compare("outstanding") == 0) {
      return congestionAlgorithm_p->getOutstanding();
      }*/
}


void Flow::resetStats() {
    dataSent = 0;
    dataReceived = 0;
    totalPacketDelay = 0;
    packetsReceived = 0;
    rtts.clear();
}

void Flow::updateDataSent(int bytes) {
    dataSent += bytes;
}

void Flow::updateDataReceived(int bytes) {
    dataReceived += bytes;
}

double Flow::getRTT() {
    double avg = 0;
    for (std::list<double>::iterator it = rtts.begin(); it != rtts.end(); it++)
    {
        avg += *it;
    }
    return avg / rtts.size();
}

void Flow::updateRTT(double rtt) {
    rtts.push_back(rtt);
}

void Flow::updateTotalPacketDelay(double delay) {
    totalPacketDelay += delay;
    packetsReceived++;
    totalPacketsRecieved++;
}


std::string Flow::infoString(){
    std::stringstream ss;
    ss << "(Flow " << this->getId() << ")";
    return ss.str();
}


int Flow::getTotalPackets(){
    return totalPackets;
}

int Flow::getProgress() {
    return progress;
}

void Flow::checkAllRecieved(int sendNext) {
    for (int i = 0; i < sendNext; i++) {
        std::map<int, double>::iterator it = packets.find(i);
        if (it == packets.end()) {
            std::cout << "Packet at index " << i << " never sent.\n";
            exit(1);
        }
        if (packets[i] != DBL_MAX) {
            std::cout << "Packet at index " << i << " was never recieved.\n";
            exit(1);
        }
    }
}

void Flow::printRemainingPacketIds() {
    std::ofstream file;
    file.open("flow.txt");
    for (std::map<int, double>::iterator it = packets.begin();
            it != packets.end(); it++)
    {
        int i = it->first;
        double time = it->second;
        if (time == 0) {
            file << i << "\n";
        }        
    }
    file << "\n\n\n";
    file.close();
}


void Flow::end() {
    done = true;
    SYSTEM_CONTROLLER->decrementFlowsLeft();
}
