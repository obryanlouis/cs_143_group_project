// read_input.cpp

// Read in the input from an xml document containing the network specifications
// following the schema described in network_schema

#include "pugixml.cpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
    // input should just have one argument, and it should be the xml file
    // containing the specification

    if (argc != 2) {
        cout << "usage: read_input inputFile" << endl;
        exit(1);
    }

    char* input = argv[1];

    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file(input);

    cout << "Load result: "  << result.description() << endl;
    
    pugi::xml_node network = doc.child("Network");

    cout << network.name() << "-------------" << endl;

    // network elements

    // snapshot time
    cout << "snapshot time:             "
         << network.child("snapshotTime").text().as_int()
         << endl;

    // routing table update time
    cout << "routing table update time: "
         << network.child_value("routingTableUpdateTime")
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
    }

    // network links
    cout << "network links: " << endl;
    pugi::xml_node networkLinks = network.child("networkLinks");
    for (pugi::xml_node link = networkLinks.first_child();
         link; link = link.next_sibling())
    {
        // first look at id
        pugi::xml_attribute id = link.attribute("id");
        cout << "    " << link.name() << ": "
             << id.name() << "=" << id.value() << endl;

        // next look at the other elements of the link
        pugi::xml_node end1 = link.child("linkRate");
        std::cout << "          " << end1.name()
                  << "=" << end1.child_value() << std::endl;

        pugi::xml_node end2 = link.child("node1");
        pugi::xml_attribute node1_type = end2.attribute("type");
        std::cout << "          " << end2.name() << ":" << std::endl;
        std::cout << "              " << node1_type.name()
                  << "=" << node1_type.value() << std::endl;
        std::cout << "              id"
                  << "=" << end2.child_value() << std::endl;

        for (pugi::xml_node elem = link.first_child();
             elem; elem = elem.next_sibling())
        {
            cout << "          " << elem.name()
                 << "=" << elem.child_value() << endl;
        }
    }


    // network flows
    cout << "network flows: " << endl;
    pugi::xml_node networkFlows = network.child("networkFlows");
    for (pugi::xml_node flow = networkFlows.first_child();
         flow; flow = flow.next_sibling())
    {
        pugi::xml_attribute id = flow.attribute("id");
        cout << "    " << flow.name() << ": "
             << id.name() << "=" << id.value() << endl;

        // next look at the other elements of the flow
        for (pugi::xml_node elem = flow.first_child();
             elem; elem = elem.next_sibling())
        {
            cout << "          " << elem.name()
                 << "=" << elem.child_value() << endl;
        }
    }

    return 0;
}
