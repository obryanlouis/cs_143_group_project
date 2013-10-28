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

    pugi::xml_node network = doc.child("network");

    for (pugi::xml_node node = network.first_child();
         node; node = node.next_sibling())
    {
        cout << node.name() << ": ";

        for (pugi::xml_attribute attr = node.first_attribute();
             attr; attr = attr.next_attribute())
        {
            cout << " " << attr.name() << "=" << attr.value();
        }

        cout << endl;
    }

    return 0;
}
