#include "Packet.h"

using namespace std;

Packet::~Packet() {
    delete data;
}

Packet::Packet(char *d, PacketType t, Host *s, Host *de, time_t start,
        int size, Flow *f) {
    data = d;
    type = t;
    source = s;
    destination = d;
    startTime = start;
    size = size;
    flowId = f;
}

const char * Packet::getData() {
    return data;
}

PacketType Packet::getType() {
    return type;
}

void Packet::setData(char * d) {
    data = d;
}
