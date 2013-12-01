// Buffer.h

#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <queue>
#include "Packet.h"

class Buffer {
private:
    std::queue<Packet*> buffer; 
        // The queue of packets in the buffer
public:
    double capacityUsed;
        // The capacity of the buffer that is used.
    double nextFree;
        // Next free time the buffer will be able to send.
    void push(Packet *p);
        // Pushes a packet onto the buffer
    void pop();
        // Pops the next element from the buffer
    Packet *front();
        // Returns the next element to be popped from the buffer
    int id;
        // ID of the buffer
    std::string infoString();
        // Returns a string formatted as: "(Buffer x)",
        // where x is the ID of the buffer.
    Buffer(int id);
        // Creates an object of class Buffer with the given specifications.
};

#endif

