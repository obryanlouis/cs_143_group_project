// Main.cpp

#include "Main.h"
#include <iostream>
#include <string>
#include <list>

using namespace std;

extern Controller *SYSTEM_CONTROLLER; 

int main(int argc, char **argv) {
    // input should just have one argument, and it should be the xml file
    // containing the specification

    if (argc != 2) {
      cout << "usage: bin/test inputFile" << endl;
      exit(1);
      }

    char* input = argv[1];

    // Initialize the System Controller
    SYSTEM_CONTROLLER = new Controller();

    SYSTEM_CONTROLLER->setInputFile(input);

    SYSTEM_CONTROLLER->run();

    return 0;
}

