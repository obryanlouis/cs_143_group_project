// Buffer.cpp

#include "Buffer.h"

void Buffer::push(Packet *p) {
    buffer.push(p);
}

void Buffer::pop() {
    buffer.pop();
}

Packet * Buffer::front() {
    return buffer.front();
}

std::string Buffer::infoString() {
    std::stringstream ss;
    ss << "(Buffer " << id << ")\n"; 
    return ss.str();
}

Buffer::Buffer(int id)
    : id(id)
    , nextFree(0)
    , capacityUsed(0)
{}

