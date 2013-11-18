#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>

#include "Control.h"


extern Controller *SYSTEM_CONTROLLER;


/*
 *  Plots all the figures.
 */
void makePlots() {

    std::cout << "Making plots...\n";
    std::cout << "Making Flow plots...\n";
    
    std::list<std::pair<std::string, std::string > > files;
    files.push_back(std::make_pair<std::string, std::string>(FLOW_RECEIVE_FILE,
                "Flow_Receive_Rates.svg"));
    files.push_back(std::make_pair<std::string, std::string>(FLOW_SEND_FILE,
                "Flow_Send_Rates.svg"));
    files.push_back(std::make_pair<std::string, std::string>(FLOW_RTT_FILE,
                "Flow_RTT.svg"));

    for (std::list<std::pair<std::string, std::string > >::iterator iter = files.begin();
        iter != files.end(); iter++)
    {
        std::string saveAs = "Output/" + iter->second;
        std::string dataFile = iter->first;
        FILE * fp = popen("gnuplot ", "w");    

        // Plot Flows
        std::vector<std::string > flowCommands;
        flowCommands.push_back("set terminal svg");
        flowCommands.push_back("set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb\"white\" behind");
        flowCommands.push_back("set output \"" + saveAs + "\"");
        std::stringstream multiplotCommand;
        multiplotCommand << "set multiplot layout ";
        multiplotCommand << SYSTEM_CONTROLLER->flows.size() << ",1";
        flowCommands.push_back(multiplotCommand.str());
        flowCommands.push_back("set xlabel \"Time\"");
        flowCommands.push_back("set ylabel \"Rate\"");
        for (std::vector<std::string>::iterator it = flowCommands.begin();
            it != flowCommands.end(); it++)
        {
            fputs(it->data(), fp); 
            fputs("\n", fp);
        }

        int k = 2;
        for (std::list<Flow* >::iterator it = SYSTEM_CONTROLLER->flows.begin();
            it != SYSTEM_CONTROLLER->flows.end(); it++)
        {
            std::stringstream command;
            command << "plot \""
                    << dataFile
                    << "\""
                    << " using 1:" << k++ << " with steps title \""
                    << (*it)->getId()
                    << "\"\n";
            fputs(command.str().data(), fp);
        }

        pclose(fp);
    }
    /*
    std::cout << "Making Link plots...\n";
    
    files.clear();
    files.push_back(std::make_pair<std::string, std::string>(LINK_OCCUPANCY_FILE,
                "Link_Occupancy.svg"));
    files.push_back(std::make_pair<std::string, std::string>(LINK_PACKET_LOSS_FILE,
                "Link_Packet_Loss.svg"));
    files.push_back(std::make_pair<std::string, std::string>(LINK_FLOW_RATE_FILE,
                "Link_Flow_Rate.svg"));

    for (std::list<std::pair<std::string, std::string > >::iterator iter = files.begin();
        iter != files.end(); iter++)
    {
        std::string saveAs = "Output/" + iter->second;
        std::string dataFile = iter->first;
        FILE * fp = popen("gnuplot ", "w");    

        // Plot Links
        std::vector<std::string > flowCommands;
        flowCommands.push_back("set terminal svg");
        flowCommands.push_back("set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb\"white\" behind");
        flowCommands.push_back("set output \"" + saveAs + "\"");
        std::stringstream multiplotCommand;
        multiplotCommand << "set multiplot layout ";
        multiplotCommand << SYSTEM_CONTROLLER->flows_p->size() << ",1";
        flowCommands.push_back(multiplotCommand.str());
        flowCommands.push_back("set xlabel \"Time\"");
        flowCommands.push_back("set ylabel \"Rate\"");
        for (std::vector<std::string>::iterator it = flowCommands.begin();
            it != flowCommands.end(); it++)
        {
            fputs(it->data(), fp); 
            fputs("\n", fp);
        }

        int k = 2;
        for (std::list<Flow* >::iterator it = SYSTEM_CONTROLLER->flows_p->begin();
            it != SYSTEM_CONTROLLER->flows_p->end(); it++)
        {
            std::stringstream command;
            command << "plot \""
                    << dataFile
                    << "\""
                    << " using 1:" << k++ << " with steps title \""
                    << (*it)->getId()
                    << "\"\n";
            fputs(command.str().data(), fp);
        }

        pclose(fp);
    }*/


}

