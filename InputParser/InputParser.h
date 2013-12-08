// InputParser.h

#ifndef INPUTPARSER_H
#define INPUTPARSER_H

// Class and helpers to parse the given network description xml file and store
// the input in some objects. The parsing is done with the open source
// C++ XML processing library pugixml (http://pugixml.org).

#include "CongestionAlgorithm.h"
#include "pugixml.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <list>

class PlotOptions;

// The base class to store information
class SystemInfo { 
public:
    int    print;
        // Print this info?
    SystemInfo(int print);
};

// Some classes to store the object info for:
class HostInfo : public SystemInfo {
public:
    int hostId;
        // ID of the host

    HostInfo(int print, int id);
        // create an instance of class HostInfo with the given specifications

    int getId();
        // get the ID of this host
};

class RouterInfo : public SystemInfo {
public:
    int routerId;
        // ID of the router

    RouterInfo(int id);
        // create an instance of class RouterInfo with the given specifications
};

class LinkInfo : public SystemInfo {
public:
    int    linkId;
        // ID of the link
    double linkRate;
        // the rate of the link in Mbps
    int    linkDelay;
        // the propagation delay of the link in ms
    int    bufferSize;
        // the size of the buffer in kilobytes
    int    node1Type;
        // the type of the first node of the link
        // 0 = host, 1 = router
    int    node1Id;
        // the id of first node of the link
    int    node2Type;
        // the type of the second node of the link
        // 0 = host, 1 = router
    int    node2Id;
        // the id of second node of the link

    LinkInfo(int print, int id, double rate, int delay, int size, int n1t,
            int n1id, int n2t, int n2id);
        // create an instance of class LinkInfo with the given specifications

    int getId();
        // get the ID of this Link
};

class FlowInfo : public SystemInfo {
public:
    int flowId;
        // ID of the flow
    int sourceId;
        // the id of the source host of the flow
    int destinationId;
        // the id of the destination host of the flow
    double flowSize;
        // the size of the flow, in megabytes
    double startTime;
        // the start time of the flow, in seconds
    CongestionAlgorithmType congestionAlgorithmType;

    FlowInfo(int print, int id, int src, int dst, double size, double start, 
            CongestionAlgorithmType congestionAlgorithmType);
        // create an instance of class FlowInfo with the given specifications

    int getId();
        // get the ID of this Host
};

class InputParser {

    std::string inputNetwork;
        // The XML file given describing the network to create.


public:
    InputParser(std::string network);
        // Create an object of class InputParser with a filename
        // of XML file holding network details
    ~InputParser();
        // Destroy this instance of InputParser

    void run(int                   &snapshotTime,
             int                   &routingUpdateTime,
             std::list<HostInfo>   &hosts,
             std::list<RouterInfo> &routers,
             std::list<LinkInfo>   &links,
             std::list<FlowInfo>   &flows,
             PlotOptions           &plotOptions);
        // Run the parser on the network XML file. Stores the data into
        // the provided arguments
};

class PlotOptions {
    public:
        bool separateLinkGraphs;
        bool separateHostGraphs;
        bool separateFlowGraphs;
};

#endif

