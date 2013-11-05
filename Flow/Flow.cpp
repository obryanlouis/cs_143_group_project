// Flow.cpp

#include "Flow.h"

Flow::Flow(int in_ID, int in_size, Host *in_source, Host *in_destination)
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
    donePackets.resize(totalPackets);
    for (int i = 0; i < totalPackets; i++){
        donePackets[i] = false;
    }
}

Flow::~Flow()
{}

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

