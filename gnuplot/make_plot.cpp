#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>

#include "Control.h"


extern Controller *SYSTEM_CONTROLLER;


/*
 *  Plots all the figures.
 */
void makePlots() {

    std::cout << "Making plots...\n";
    std::cout << "Making Flow plots...\n";
    
    std::list<std::tuple<std::string, std::string, std::string > > files;
    files.push_back(std::make_tuple(FLOW_RECEIVE_FILE,
                "Flow_Receive_Rates.svg", "Mbps"));
    files.push_back(std::make_tuple(FLOW_SEND_FILE,
                "Flow_Send_Rates.svg", "Mbps"));
    files.push_back(std::make_tuple(FLOW_RTT_FILE,
                "Flow_RTT.svg", "ms"));

    for (std::list<std::tuple<std::string, std::string, std::string > >::iterator iter = files.begin();
        iter != files.end(); iter++)
    {
        std::string saveAs = "Output/" + std::get<1>(*iter);
        std::string dataFile = std::get<0>(*iter);
        std::string ylabel = std::get<2>(*iter);
        FILE * fp = popen("gnuplot ", "w");    

        // Plot Flows
        std::vector<std::string > commands;
        commands.push_back("set terminal svg size 1000,550");
        commands.push_back("set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb\"white\" behind");
        commands.push_back("set output \"" + saveAs + "\"");
        std::stringstream multiplotCommand;
        multiplotCommand << "set multiplot layout ";
        multiplotCommand << SYSTEM_CONTROLLER->flows.size() << ",1";
        commands.push_back(multiplotCommand.str());
        commands.push_back("set xlabel \"Time (s)\"");
        commands.push_back("set ylabel \"" + ylabel + "\"");
        for (std::vector<std::string>::iterator it = commands.begin();
            it != commands.end(); it++)
        {
            fputs(it->data(), fp); 
            fputs("\n", fp);
        }

        int k = 2;
        for (std::list<Flow* >::iterator it = SYSTEM_CONTROLLER->flows.begin();
            it != SYSTEM_CONTROLLER->flows.end(); it++)
        {
            std::stringstream command;
            if (k == 3)
                command << "unset object 1\n";
            command << "plot \""
                    << dataFile
                    << "\""
                    << " using 1:" << k++ << " with points pointtype 7 pointsize 0.5 title \""
                    << (*it)->infoString()
                    << "\"\n";
            fputs(command.str().data(), fp);
        }

        pclose(fp);
    }

    std::cout << "Making Link plots...\n";
    
    files.clear();
    files.push_back(std::make_tuple(LINK_OCCUPANCY_FILE,
                "Link_Occupancy.svg", "Bytes"));
    files.push_back(std::make_tuple(LINK_PACKET_LOSS_FILE,
                "Link_Packet_Loss.svg", "Mbps"));
    files.push_back(std::make_tuple(LINK_FLOW_RATE_FILE,
                "Link_Flow_Rate.svg", "Mbps"));

    for (std::list<std::tuple<std::string, std::string, std::string > >::iterator iter = files.begin();
        iter != files.end(); iter++)
    {
        std::string saveAs = "Output/" + std::get<1>(*iter);
        std::string dataFile = std::get<0>(*iter);
        std::string ylabel = std::get<2>(*iter);
        FILE * fp = popen("gnuplot ", "w");    

        // Plot Links
        std::vector<std::string > commands;
        commands.push_back("set terminal svg size 1000,550");
        commands.push_back("set output \"" + saveAs + "\"");
        commands.push_back("set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb\"white\" behind");
        std::stringstream multiplotCommand;
        multiplotCommand << "set multiplot layout ";
        multiplotCommand << SYSTEM_CONTROLLER->numLinksToPrint() << ",1";
        commands.push_back(multiplotCommand.str());
        commands.push_back("set xlabel \"Time (s)\"");
        commands.push_back("set ylabel \"" + ylabel + "\"");
        int i = 1;
        for (std::vector<std::string>::iterator it = commands.begin();
            it != commands.end(); it++)
        {
            fputs(it->data(), fp); 
            fputs("\n", fp);
        }

        int k = 2;
        for (std::list<LinkInfo >::iterator it = SYSTEM_CONTROLLER->linkInfos.begin();
            it != SYSTEM_CONTROLLER->linkInfos.end(); it++)
        {
            if (it->print) {
                std::stringstream command;
                if (k == 3)
                    command << "unset object 1\n";
                command << "plot \""
                        << dataFile
                        << "\""
                        << " using 1:" << k++ << " with points pointtype 7 pointsize 0.5 title \""
                        << it->linkId
                        << "\"\n";
                fputs(command.str().data(), fp);
            }
        }

        pclose(fp);
    }


    std::cout << "Making Host plots...\n";
    
    files.clear();
    files.push_back(std::make_tuple(HOST_SEND_FILE,
                "Host_Send.svg", "Mbps"));
    files.push_back(std::make_tuple(HOST_RECEIVE_FILE,
                "Host_Receive.svg", "Mbps"));

    for (std::list<std::tuple<std::string, std::string, std::string > >::iterator iter = files.begin();
        iter != files.end(); iter++)
    {
        std::string saveAs = "Output/" + std::get<1>(*iter);
        std::string dataFile = std::get<0>(*iter);
        std::string ylabel = std::get<2>(*iter);
        FILE * fp = popen("gnuplot ", "w");    

        // Plot Hosts
        std::vector<std::string > commands;
        commands.push_back("set terminal svg size 1000,550");
        commands.push_back("set output \"" + saveAs + "\"");
        commands.push_back("set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb\"white\" behind");
        std::stringstream multiplotCommand;
        multiplotCommand << "set multiplot layout ";
        multiplotCommand << SYSTEM_CONTROLLER->hosts.size() << ",1";
        commands.push_back(multiplotCommand.str());
        commands.push_back("set xlabel \"Time (s)\"");
        commands.push_back("set ylabel \"" + ylabel + "\"");
        int i = 1;
        for (std::vector<std::string>::iterator it = commands.begin();
            it != commands.end(); it++)
        {
            fputs(it->data(), fp); 
            fputs("\n", fp);
        }

        int k = 2;
        for (std::list<Host* >::iterator it = SYSTEM_CONTROLLER->hosts.begin();
            it != SYSTEM_CONTROLLER->hosts.end(); it++)
        {
            std::stringstream command;
            if (k == 3)
                command << "unset object 1\n";
            command << "plot \""
                    << dataFile
                    << "\""
                    << " using 1:" << k++ << " with points pointtype 7 pointsize 0.5 title \""
                    << (*it)->infoString()
                    << "\"\n";
            fputs(command.str().data(), fp);
        }

        pclose(fp);
    }

}

