#include "Packet.h"

using namespace std;

Packet::~Packet() {
    delete data;
}

Packet::Packet(char *d, Packet::PacketType t, Host *s, Host *de, time_t start,
        int size, Flow *f, string id) {
    data = d;
    type = t;
    source = s;
    destination = de;
    startTime = start;
    size = size;
    flowId = f;
    id = id;
}

const char * Packet::getData() {
    return data;
}

Packet::PacketType Packet::getType() {
    return type;
}

void Packet::setData(char * d) {
    data = d;
}
