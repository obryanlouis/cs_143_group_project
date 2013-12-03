#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>

#include "Control.h"
#include "InputParser.h"


extern Controller *SYSTEM_CONTROLLER;

enum StatType {
    FLOWSTAT,
    LINKSTAT,
    HOSTSTAT
};

template <typename Iterator>
void graphAll(Iterator first, Iterator second, std::string dataFile,
        FILE *fp) {
    int k = 2;
    for (Iterator it = first ; it != second; it++)
    {
        if (it->print) {
            std::stringstream command;
            if (k == 3)
                command << "unset object 1\n";
            command << "plot \""
                    << dataFile
                    << "\""
                    << " using 1:" << k++ << " with points pointtype 7 pointsize 0.5 title \""
                    << it->getId()
                    << "\"\n";
            fputs(command.str().data(), fp);
        }
    }
}

void makePlots() {

    std::cout << "Making plots...\n";

    // Store the filename and y axis label for each statistic in a list
    std::list<std::tuple<std::string, std::string, std::string,
       StatType > > files;

    files.push_back(std::make_tuple(FLOW_RECEIVE_FILE,
                "Flow_Receive_Rates.svg", "Mbps", FLOWSTAT));
    files.push_back(std::make_tuple(FLOW_SEND_FILE,
                "Flow_Send_Rates.svg", "Mbps", FLOWSTAT));
    files.push_back(std::make_tuple(FLOW_RTT_FILE,
                "Flow_RTT.svg", "ms", FLOWSTAT));
    files.push_back(std::make_tuple(FLOW_WINDOW_FILE,
                "Flow_Window.svg","Packets", FLOWSTAT));
    files.push_back(std::make_tuple(FLOW_THRESH_FILE,
                "Flow_Thresh.svg","Packets", FLOWSTAT));
    files.push_back(std::make_tuple(FLOW_OUTSTANDING_FILE,
                "Flow_Outstanding.svg","Packets", FLOWSTAT));

    files.push_back(std::make_tuple(LINK_OCCUPANCY_FILE,
                "Link_Occupancy.svg", "Bytes", LINKSTAT));
    files.push_back(std::make_tuple(LINK_PACKET_LOSS_FILE,
                "Link_Packet_Loss.svg", "Mbps", LINKSTAT));
    files.push_back(std::make_tuple(LINK_FLOW_RATE_FILE,
                "Link_Flow_Rate.svg", "Mbps", LINKSTAT));

    files.push_back(std::make_tuple(HOST_SEND_FILE,
                "Host_Send.svg", "Mbps", HOSTSTAT));
    files.push_back(std::make_tuple(HOST_RECEIVE_FILE,
                "Host_Receive.svg", "Mbps", HOSTSTAT));

    // Iterate through each statistic and plot it with gnuplot
    for (std::list<std::tuple<std::string, std::string, std::string, 
            StatType > >::iterator iter = files.begin();
        iter != files.end(); iter++)
    {
        std::string saveAs = "Output/" + std::get<1>(*iter);
        // Retrieve the file name
        std::string dataFile = std::get<0>(*iter);
        // Retrieve the y axis label
        std::string ylabel = std::get<2>(*iter);
        FILE * fp = popen("gnuplot ", "w");    

        // Execute gnuplot commands common for each stat file
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

        // Depending on the statistic type being graphed, graph all
        // flows / links / hosts for the given statistic 
        StatType t = std::get<3>(*iter);
        std::list<SystemInfo>::iterator it;
        std::list<SystemInfo>::iterator end;
        if (t == FLOWSTAT) {
            graphAll(SYSTEM_CONTROLLER->flowInfos.begin(),
                    SYSTEM_CONTROLLER->flowInfos.end(),
                    dataFile, fp);
        }
        else if (t == HOSTSTAT) {
            graphAll(SYSTEM_CONTROLLER->hostInfos.begin(),
                    SYSTEM_CONTROLLER->hostInfos.end(),
                    dataFile, fp);
        }
        else if (t == LINKSTAT) {
            graphAll(SYSTEM_CONTROLLER->linkInfos.begin(),
                    SYSTEM_CONTROLLER->linkInfos.end(),
                    dataFile, fp);
        }

        pclose(fp);
    }

}

