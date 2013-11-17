// read_input.cpp

// Read in the input from an xml document containing the network specifications
// following the schema described in network_schema

#include "pugixml.cpp"
#include <iostream>
#include <string>
#include <list>

using namespace std;

// Some structs to store the object info for:
struct HostInfo {
    int hostId;
        // ID of the host

    HostInfo(int id);
        // create an instance of class HostInfo with the given specifications
};

struct RouterInfo {
    int routerId;
        // ID of the router

    RouterInfo(int id);
        // create an instance of class RouterInfo with the given specifications
};

struct LinkInfo {
    int linkId;
        // ID of the link
    int linkRate;
        // the rate of the link in Mbps
    int linkDelay;
        // the propagation delay of the link in seconds
    int bufferSize;
        // the size of the buffer in bytes
    int node1Type;
        // the type of the first node of the link
        // 0 = host, 1 = router
    int node1Id;
        // the id of first node of the link
    int node2Type;
        // the type of the second node of the link
        // 0 = host, 1 = router
    int node2Id;
        // the id of second node of the link

    LinkInfo(int id, int rate, int delay, int size, int n1t,
            int n1id, int n2t, int n2id);
        // create an instance of class LinkInfo with the given specifications
};

struct FlowInfo {
    int flowId;
        // ID of the flow
    int sourceId;
        // the id of the source host of the flow
    int destinationId;
        // the id of the destination host of the flow
    int flowSize;
        // the size of the flow, in bytes;
    int startTime;
        // the start time of the flow, in seconds

    FlowInfo(int id, int src, int dst, int size, int start);
        // create an instance of class FlowInfo with the given specifications
};

HostInfo::HostInfo(int id)
    : hostId(id)
{}

RouterInfo::RouterInfo(int id)
    : routerId(id)
{}

LinkInfo::LinkInfo(int id, int rate, int delay, int size, int n1t,
        int n1id, int n2t, int n2id)
    : linkId(id)
    , linkRate(rate)
    , linkDelay(delay)
    , bufferSize(size)
    , node1Type(n1t)
    , node1Id(n1id)
    , node2Type(n2t)
    , node2Id(n2id)
{}

FlowInfo::FlowInfo(int id, int src, int dst, int size, int start)
    : flowId(id)
    , sourceId(src)
    , destinationId(dst)
    , flowSize(size)
    , startTime(start)
{}

Node * getNode(int type, int id, map<int, Host* > hostsById,
        map<int, Router* > routersById)
{
    if (type == 0) {
        return hostsById[id];
    }
    else if (type == 1) {
        return routersById[id];
    }
    std::cout << "Invalid node type: " << type << "\n";
    exit(1);
}

int main(int argc, char **argv) {
    // input should just have one argument, and it should be the xml file
    // containing the specification

    if (argc != 2) {
        cout << "usage: read_input inputFile" << endl;
        exit(1);
    }

    char* input = argv[1];

    int              snapshotTime;
    int              routingUpdateTime;
    list<HostInfo>   hosts;
    list<RouterInfo> routers;
    list<LinkInfo>   links;
    list<FlowInfo>   flows;

    pugi::xml_document doc;
        // create an xml_document to load the file

    pugi::xml_parse_result result = doc.load_file(input);

    cout << "**************************************************"
              << endl
              << "Starting to parse input file: "
              << input
              << "."
              << "**************************************************"
              << endl << endl;

    cout << "Load result: "  << result.description() << endl;
    
    pugi::xml_node network = doc.child("Network");

    cout << network.name() << "-------------" << endl;

    // network elements

    // snapshot time
    snapshotTime = network.child("snapshotTime").text().as_int();
    cout << "snapshot time:             "
         << snapshotTime
         << endl;

    // routing table update time
    routingUpdateTime = network.child("routingTableUpdateTime").text().as_int();
    cout << "routing table update time: "
         << routingUpdateTime
         << endl;

    // network hosts
    cout << "network hosts: " << endl;
    pugi::xml_node networkHosts = network.child("networkHosts");
    for (pugi::xml_node host = networkHosts.first_child();
         host; host = host.next_sibling())
    {
        pugi::xml_attribute id = host.attribute("id");
        cout << "    " << host.name() << ": "
             << id.name() << "=" << id.value() << endl;

        HostInfo info(id.as_int());
        hosts.push_back(info);
    }

    // network routers
    cout << "network routers: " << endl;
    pugi::xml_node networkRouters = network.child("networkRouters");
    for (pugi::xml_node router = networkRouters.first_child();
         router; router = router.next_sibling())
    {
        pugi::xml_attribute id = router.attribute("id");
        cout << "    " << router.name() << ": "
             << id.name() << "=" << id.value() << endl;

        RouterInfo info(id.as_int());
        routers.push_back(info);
    }

    // network links
    cout << "network links: " << endl;
    pugi::xml_node networkLinks = network.child("networkLinks");
    for (pugi::xml_node link = networkLinks.first_child();
         link; link = link.next_sibling())
    {
        // link id
        pugi::xml_attribute id_att = link.attribute("id");
        cout << "    " << link.name() << ": "
             << id_att.name() << "=" << id_att.value() << endl;

        // link rate
        pugi::xml_node rate_node = link.child("linkRate");
        cout << "          " << rate_node.name()
                  << "=" << rate_node.child_value() << endl;

        // link delay
        pugi::xml_node delay_node = link.child("linkDelay");
        cout << "          " << delay_node.name()
                  << "=" << delay_node.child_value() << endl;

        // buffer size
        pugi::xml_node size_node = link.child("bufferSize");
        cout << "          " << size_node.name()
                  << "=" << size_node.child_value() << endl;

        // node 1
        pugi::xml_node node1_node = link.child("node1");
        pugi::xml_attribute node1_type = node1_node.attribute("type");
        cout << "          " << node1_node.name() << ":" << endl;
        cout << "              " << node1_type.name()
                  << "=" << node1_type.value() << endl;
        cout << "              id"
                  << "=" << node1_node.child_value() << endl;

        // node 2
        pugi::xml_node node2_node = link.child("node2");
        pugi::xml_attribute node2_type = node2_node.attribute("type");
        cout << "          " << node2_node.name() << ":" << endl;
        cout << "              " << node2_type.name()
                  << "=" << node2_type.value() << endl;
        cout << "              id"
                  << "=" << node2_node.child_value() << endl;

        LinkInfo info(
                id_att.as_int(),
                atoi(rate_node.child_value()),
                atoi(delay_node.child_value()),
                atoi(size_node.child_value()),
                node1_type.as_int(),
                atoi(node1_node.child_value()),
                node2_type.as_int(),
                atoi(node2_node.child_value()));

        links.push_back(info);
    }

    // network flows
    cout << "network flows: " << endl;
    pugi::xml_node networkFlows = network.child("networkFlows");
    for (pugi::xml_node flow = networkFlows.first_child();
         flow; flow = flow.next_sibling())
    {
        // flow id
        pugi::xml_attribute id_att = flow.attribute("id");
        cout << "    " << flow.name() << ": "
             << id_att.name() << "=" << id_att.value() << endl;

        // source host
        pugi::xml_node src_node = flow.child("source");
        cout << "          " << src_node.name()
                  << "=" << src_node.child_value() << endl;

        // destination host
        pugi::xml_node dst_node = flow.child("destination");
        cout << "          " << dst_node.name()
                  << "=" << dst_node.child_value() << endl;

        // flow size
        pugi::xml_node size_node = flow.child("flowSize");
        cout << "          " << size_node.name()
                  << "=" << size_node.child_value() << endl;

        // flow start time
        pugi::xml_node time_node = flow.child("flowStartTime");
        cout << "          " << time_node.name()
                  << "=" << time_node.child_value() << endl;

        FlowInfo info(
                id_att.as_int(),
                atoi(src_node.child_value()),
                atoi(dst_node.child_value()),
                atoi(size_node.child_value()),
                atoi(time_node.child_value()));

        flows.push_back(info);
    }

    cout << "**************************************************"
         << endl
         << "Done parsing input file: "
         << input
         << "."
         << "**************************************************"
         << endl << endl;
              
    // Create the objects to be used in the system
    int              snapshotTime;
    int              routingUpdateTime;
    list<HostInfo>   hosts;
    list<RouterInfo> routers;
    list<LinkInfo>   links;
    list<FlowInfo>   flows;

    std::map<int, Host* > hostsById;
    std::map<int, Router* > routersById;

    for (list<HostInfo>::iterator it = hosts.begin(); it != hosts.end(); it++)
    {
        Host *h = new Host(it->hostId);
        hostsById[it->hostId] = h;
        SYSTEM_CONTROLLER->addHost(h);
    }
 
    for (list<FlowInfo>::iterator it = hosts.begin(); it != hosts.end(); it++)
    {
        Flow *f = new Flow(it->flowId, it->flowSize, it->sourceId,
                it->destinationId);
        SYSTEM_CONTROLLER->addFlow(f);
    }

    for (list<RouterInfo>::iterator it = routers.begin(); it != routers.end();
            it++)
    {
        Router *r = new Router(it->routerId);
        routersById[it->routerId] = it->routerId;
        SYSTEM_CONTROLLER->addRouter(r);
    }

    for (list<LinkInfo>::iterator it = links.begin(); it != links.end(); it++)
    {
        Node *n1 = getNode(it->nodeType1, it->nodeId1, hostsById, routersById);
        Node *n2 = getNode(it->nodeType2, it->nodeId2, hostsById, routersById);
        Link *l = new Link(it->linkId, n1, n2, it->bufferSize, it->linkRate,
                it->linkDelay);
        SYSTEM_CONTROLLER->addLink(l);
    }

    // print some stuff
    cout << "snapshot time:     " << snapshotTime      << endl;
    cout << "routingUpdateTime: " << routingUpdateTime << endl;
    cout << "There are " << hosts.size() << " hosts:" << endl;
    for (list<HostInfo>::iterator it = hosts.begin();
         it != hosts.end(); it++)
    {
        cout << "    "
             << "id="
             << it->hostId
             << endl;
    }
    cout << "There are " << routers.size() << " routers:" << endl;
    for (list<RouterInfo>::iterator it = routers.begin();
         it != routers.end(); it++)
    {
        cout << "   "
             << "id="
             << it->routerId
             << endl;
    }
    cout << "There are " << links.size() << " links:" << endl;
    cout << "Type 0 = host;  type 1 = router" << endl;
    for (list<LinkInfo>::iterator it = links.begin();
         it != links.end(); it++)
    {
        cout << "    " << "id=" << it->linkId << endl
             << "    " << "rate=" << it->linkRate << endl
             << "    " << "delay=" << it->linkDelay << endl
             << "    " << "bufferSize=" << it->bufferSize << endl
             << "    " << "node1Type=" << it->node1Type << endl
             << "    " << "node1Id=" << it->node1Id << endl
             << "    " << "node2Type=" << it->node2Type << endl
             << "    " << "node2Id=" << it->node2Id << endl;
    }
    cout << "There are " << flows.size() << " flows:" << endl;
    for (list<FlowInfo>::iterator it = flows.begin();
         it != flows.end(); it++)
    {
        cout << "    " << "id=" << it->flowId << endl
             << "    " << "sourceId=" << it->sourceId << endl
             << "    " << "destinationId=" << it->destinationId << endl
             << "    " << "flowSize=" << it->flowSize << endl
             << "    " << "startTime=" << it->startTime << endl;
    }

    return 0;
}
