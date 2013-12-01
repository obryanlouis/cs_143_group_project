// InputParser.cpp

#include "InputParser.h"

HostInfo::HostInfo(int id)
    : hostId(id)
{}

RouterInfo::RouterInfo(int id)
    : routerId(id)
{}

LinkInfo::LinkInfo(int print, int id, double rate, int delay, int size,
        int n1t, int n1id, int n2t, int n2id)
    : print(print)
    , linkId(id)
    , linkRate(rate)
    , linkDelay(delay)
    , bufferSize(size)
    , node1Type(n1t)
    , node1Id(n1id)
    , node2Type(n2t)
    , node2Id(n2id)
{}

FlowInfo::FlowInfo(int id, int src, int dst, int size, double start,
        CongestionAlgorithmType congestionAlgorithmType)
    : flowId(id)
    , sourceId(src)
    , destinationId(dst)
    , flowSize(size)
    , startTime(start)
    , congestionAlgorithmType(congestionAlgorithmType)
{}


InputParser::InputParser(std::string network)
    : inputNetwork(network)
{}

InputParser::~InputParser()
{}

void InputParser::run(int                   &snapshotTime,
                      int                   &routingUpdateTime,
                      std::list<HostInfo>   &hosts,
                      std::list<RouterInfo> &routers,
                      std::list<LinkInfo>   &links,
                      std::list<FlowInfo>   &flows)
{
    pugi::xml_document doc;
        // create an xml_document to load the file

    pugi::xml_parse_result result = doc.load_file(inputNetwork.c_str());

    std::cout << "**************************************************"
              << std::endl
              << "Starting to parse input file: "
              << inputNetwork
              << "."
              << "**************************************************"
              << std::endl << std::endl;

    std::cout << "Load result: "  << result.description() << std::endl;
    
    pugi::xml_node network = doc.child("Network");

    std::cout << network.name() << "-------------" << std::endl;

    // network elements

    // snapshot time
    snapshotTime = network.child("snapshotTime").text().as_int();
    std::cout << "snapshot time:             "
              << snapshotTime
              << std::endl;

    // routing table update time
    routingUpdateTime = network.child("routingTableUpdateTime").text().as_int();
    std::cout << "routing table update time: "
              << routingUpdateTime
              << std::endl;

    // network hosts
    std::cout << "network hosts: " << std::endl;
    pugi::xml_node networkHosts = network.child("networkHosts");
    for (pugi::xml_node host = networkHosts.first_child();
         host; host = host.next_sibling())
    {
        pugi::xml_attribute id = host.attribute("id");
        std::cout << "    " << host.name() << ": "
             << id.name() << "=" << id.value() << std::endl;

        HostInfo info(id.as_int());
        hosts.push_back(info);
    }

    // network routers
    std::cout << "network routers: " << std::endl;
    pugi::xml_node networkRouters = network.child("networkRouters");
    for (pugi::xml_node router = networkRouters.first_child();
         router; router = router.next_sibling())
    {
        pugi::xml_attribute id = router.attribute("id");
        std::cout << "    " << router.name() << ": "
             << id.name() << "=" << id.value() << std::endl;

        RouterInfo info(id.as_int());
        routers.push_back(info);
    }

    // network links
    std::cout << "network links: " << std::endl;
    pugi::xml_node networkLinks = network.child("networkLinks");
    for (pugi::xml_node link = networkLinks.first_child();
         link; link = link.next_sibling())
    {
        // link id
        pugi::xml_attribute id_att = link.attribute("id");
        std::cout << "    " << link.name() << ": "
             << id_att.name() << "=" << id_att.value() << std::endl;

        // print
        pugi::xml_attribute print_att = link.attribute("print");
        if (print_att.as_int()) {
            std::cout << "Link "
                      << id_att.as_int()
                      << " set to print\n";
        }
        else {
            std::cout << "Link "
                      << id_att.as_int()
                      << " not set to print\n";
        }


        // link rate
        pugi::xml_node rate_node = link.child("linkRate");
        std::cout << "          " << rate_node.name()
                  << "=" << rate_node.child_value() << std::endl;

        // link delay
        pugi::xml_node delay_node = link.child("linkDelay");
        std::cout << "          " << delay_node.name()
                  << "=" << delay_node.child_value() << std::endl;

        // buffer size
        pugi::xml_node size_node = link.child("bufferSize");
        std::cout << "          " << size_node.name()
                  << "=" << size_node.child_value() << std::endl;

        // node 1
        pugi::xml_node node1_node = link.child("node1");
        pugi::xml_attribute node1_type = node1_node.attribute("type");
        std::cout << "          " << node1_node.name() << ":" << std::endl;
        std::cout << "              " << node1_type.name()
                  << "=" << node1_type.value() << std::endl;
        std::cout << "              id"
                  << "=" << node1_node.child_value() << std::endl;

        // node 2
        pugi::xml_node node2_node = link.child("node2");
        pugi::xml_attribute node2_type = node2_node.attribute("type");
        std::cout << "          " << node2_node.name() << ":" << std::endl;
        std::cout << "              " << node2_type.name()
                  << "=" << node2_type.value() << std::endl;
        std::cout << "              id"
                  << "=" << node2_node.child_value() << std::endl;

        LinkInfo info(
                print_att.as_int(),
                id_att.as_int(),
                atoi(rate_node.child_value()),
                atoi(delay_node.child_value()),
                atof(size_node.child_value()),
                node1_type.as_int(),
                atoi(node1_node.child_value()),
                node2_type.as_int(),
                atoi(node2_node.child_value()));

        links.push_back(info);
    }

    // network flows
    std::cout << "network flows: " << std::endl;
    pugi::xml_node networkFlows = network.child("networkFlows");
    for (pugi::xml_node flow = networkFlows.first_child();
         flow; flow = flow.next_sibling())
    {
        // flow id
        pugi::xml_attribute id_att = flow.attribute("id");
        std::cout << "    " << flow.name() << ": "
             << id_att.name() << "=" << id_att.value() << std::endl;

        // congestion control algorithm
        pugi::xml_node congestion = flow.child("congestionAlgorithmType");
        CongestionAlgorithmType congestionAlgorithmType;
        std::string ctext(congestion.text().get());
        if (ctext.compare("reno") == 0) {
            congestionAlgorithmType = RENO;
        }
        else if (ctext.compare("vegas") == 0) {
            congestionAlgorithmType = VEGAS;
        }
        else {
            std::cout << "Invalid congestion control algorithm: "
                      << ctext
                      << ".\n"
                      << "Options are: reno, vegas\n";
        }

        // source host
        pugi::xml_node src_node = flow.child("source");
        std::cout << "          " << src_node.name()
                  << "=" << src_node.child_value() << std::endl;

        // destination host
        pugi::xml_node dst_node = flow.child("destination");
        std::cout << "          " << dst_node.name()
                  << "=" << dst_node.child_value() << std::endl;

        // flow size
        pugi::xml_node size_node = flow.child("flowSize");
        std::cout << "          " << size_node.name()
                  << "=" << size_node.child_value() << std::endl;

        // flow start time
        pugi::xml_node time_node = flow.child("flowStartTime");
        std::cout << "          " << time_node.name()
                  << "=" << time_node.child_value() << std::endl;

        FlowInfo info(
                id_att.as_int(),
                atoi(src_node.child_value()),
                atoi(dst_node.child_value()),
                atoi(size_node.child_value()),
                atof(time_node.child_value()),
                congestionAlgorithmType);

        flows.push_back(info);
    }

    std::cout << "**************************************************"
              << std::endl
              << "Done parsing input file: "
              << inputNetwork
              << "."
              << "**************************************************"
              << std::endl << std::endl;
}

