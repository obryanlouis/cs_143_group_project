

#ifndef PACKET_H
#define PACKET_H

#include "Flow.h"
class Packet{
    private:
        enum PacketType {
            INF,
            DATA,
            ACK
        };
        char *data;
        PacketType type;
        string id;
        Host *source;
        Host *destination;
        time_t startTime;
        int size;
        Flow *flowId;

    public:
        PacketType getType();
        Packet(char *d, PacketType t, Host *s, Host *de, time_t start, 
                int size, Flow *f);
        ~Packet();
        const char * getData();
        void setData(char * d);

};

#endif
