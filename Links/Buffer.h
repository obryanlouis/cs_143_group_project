#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <queue>
#include "Packet.h"

class Buffer {
private:
    std::queue<Packet*> buffer; 
public:
    int capacityUsed;
    unsigned int nextFree;
    void push(Packet *p);
    void pop();
    Packet *front();
    int id;
    std::string infoString();
    Buffer(int id);
};

#endif
