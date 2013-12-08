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
        FILE *fp, StatType t, bool separateGraphs) {
    if (!separateGraphs) {
        int k = 2;
        std::stringstream command;
        command << "plot ";
        for (Iterator it = first ; it != second; it++)
        {
            if (it->print) {
                command << "\""
                    << dataFile
                    << "\""
                    << " using 1:" << k++ << " with points pointtype 7 pointsize 0.5 title \""
                    << it->getId()
                    << "\", ";
            }
        }
        std::string strcommand = command.str();
        strcommand = strcommand.substr(0, strcommand.length() - 2);
        fputs(strcommand.data(), fp);
    }
    else {
        std::stringstream multiplotCommand;
        multiplotCommand << "set multiplot layout ";
        if (t == FLOWSTAT) {
            multiplotCommand << SYSTEM_CONTROLLER->numFlowsToPrint() << ",1";
        }
        else if (t == HOSTSTAT) {
            multiplotCommand << SYSTEM_CONTROLLER->numHostsToPrint() << ",1";
        }
        else if (t == LINKSTAT) {
            multiplotCommand << SYSTEM_CONTROLLER->numLinksToPrint() << ",1";
        }
        multiplotCommand << "\n";
        fputs(multiplotCommand.str().data(), fp);
        int k = 2;
        std::stringstream command;
        for (Iterator it = first ; it != second; it++)
        {
            if (it->print) {
                if (k == 3)
                    command << "unset object 1\n";
                command << "plot "
                    << "\""
                    << dataFile
                    << "\""
                    << " using 1:" << k++ << " with points pointtype 7 pointsize 0.5 title \""
                    << it->getId()
                    << "\"\n";
            }
        }
        std::string strcommand = command.str();
        strcommand = strcommand.substr(0, strcommand.length() - 2);
        fputs(strcommand.data(), fp);
    }
}

void makePlots(PlotOptions *plotOptions) {

    std::cout << "Making plots...\n";

    // Store the filename and y axis label for each statistic in a list
    std::list<std::tuple<std::string, std::string, std::string,
        StatType, std::string > > files;

    files.push_back(std::make_tuple(FLOW_RECEIVE_FILE,
                "Flow_Receive_Rates.svg", "Mbps", FLOWSTAT,
                "Flow Recieve Rates"));
    files.push_back(std::make_tuple(FLOW_SEND_FILE,
                "Flow_Send_Rates.svg", "Mbps", FLOWSTAT,
                "Flow Send Rates"));
    files.push_back(std::make_tuple(FLOW_RTT_FILE,
                "Flow_RTT.svg", "ms", FLOWSTAT, "Flow Packet Delays"));
    files.push_back(std::make_tuple(FLOW_OUTSTANDING_FILE,
                "Flow_Outstanding_Packets.svg", "Packets", FLOWSTAT, "Flow Outstanding Packets"));
    files.push_back(std::make_tuple(FLOW_WINDOW_FILE,
                "Flow_Window.svg","Packets", FLOWSTAT, "Flow Window Size"));
    files.push_back(std::make_tuple(FLOW_VEGAS_FILE,
                "Vegas_Diff.svg","Packets / ms", FLOWSTAT, "Vegas Diff Parameter = cwnd * (1 / rttmin - 1 / rttcurrent)"));

    files.push_back(std::make_tuple(LINK_OCCUPANCY_FILE,
                "Link_Occupancy.svg", "Bytes", LINKSTAT, "Link Occupancy"));
    files.push_back(std::make_tuple(LINK_PACKET_LOSS_FILE,
                "Link_Packet_Loss.svg", "Mbps", LINKSTAT, "Link Packet Loss"));
    files.push_back(std::make_tuple(LINK_FLOW_RATE_FILE,
                "Link_Flow_Rate.svg", "Mbps", LINKSTAT, "Link Flow Rates"));

    files.push_back(std::make_tuple(HOST_SEND_FILE,
                "Host_Send.svg", "Mbps", HOSTSTAT, "Host Send Rates"));
    files.push_back(std::make_tuple(HOST_RECEIVE_FILE,
                "Host_Receive.svg", "Mbps", HOSTSTAT, "Host Recieve Rates"));

    // Iterate through each statistic and plot it with gnuplot
    for (std::list<std::tuple<std::string, std::string, std::string, 
            StatType, std::string > >::iterator iter = files.begin();
            iter != files.end(); iter++)
    {
        std::string saveAs = "Output/" + std::get<1>(*iter);
        // Retrieve the file name
        std::string dataFile = std::get<0>(*iter);
        // Retrieve the y axis label
        std::string ylabel = std::get<2>(*iter);
        StatType t = std::get<3>(*iter);
        std::string title = std::get<4>(*iter);
        FILE * fp = popen("gnuplot ", "w");    

        std::string keytitle = "";
        if (t == FLOWSTAT)
            keytitle = "Flows";
        else if (t == HOSTSTAT)
            keytitle = "Hosts";
        else if (t == LINKSTAT)
            keytitle = "Links";

        // Execute gnuplot commands common for each stat file
        std::vector<std::string > commands;
        commands.push_back("set title \"" + title + "\"");
        commands.push_back("set key below right box title \"" + keytitle + "\"");
        commands.push_back("set terminal svg size 1000,550");
        commands.push_back("set output \"" + saveAs + "\"");
        commands.push_back("set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb\"white\" behind");
        /*std::stringstream multiplotCommand;
          multiplotCommand << "set multiplot layout ";
          if (t == FLOWSTAT) {
          multiplotCommand << SYSTEM_CONTROLLER->numFlowsToPrint() << ",1";
          }
          else if (t == HOSTSTAT) {
          multiplotCommand << SYSTEM_CONTROLLER->numHostsToPrint() << ",1";
          }
          else if (t == LINKSTAT) {
          multiplotCommand << SYSTEM_CONTROLLER->numLinksToPrint() << ",1";
          }
          commands.push_back(multiplotCommand.str());*/
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
        std::list<SystemInfo>::iterator it;
        std::list<SystemInfo>::iterator end;
        if (t == FLOWSTAT) {
            graphAll(SYSTEM_CONTROLLER->flowInfos.begin(),
                    SYSTEM_CONTROLLER->flowInfos.end(),
                    dataFile, fp, t, plotOptions->separateFlowGraphs);
        }
        else if (t == HOSTSTAT) {
            graphAll(SYSTEM_CONTROLLER->hostInfos.begin(),
                    SYSTEM_CONTROLLER->hostInfos.end(),
                    dataFile, fp, t, plotOptions->separateHostGraphs);
        }
        else if (t == LINKSTAT) {
            graphAll(SYSTEM_CONTROLLER->linkInfos.begin(),
                    SYSTEM_CONTROLLER->linkInfos.end(),
                    dataFile, fp, t, plotOptions->separateLinkGraphs);
        }

        pclose(fp);
    }

}

