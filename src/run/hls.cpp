// HLS.cpp
#include "hls.h"
#include "gengraphgroup.h"
#include "easylogging++.h"
#include "schedule.h"
#include "binding.h"
#include "genrtl.h"
#include <chrono>
#include <iostream>

HLS::HLS(Parser& parser) : parser(parser){
}

HLS::~HLS() {
    // 释放资源代码
}

void HLS::run() {
    

    LOG(INFO) << "HLS start running";
    auto start = std::chrono::system_clock::now();
    GenGraphGroup genGraphGroup(parser, graph_group);
    LOG(INFO) << "GenGraphGroup finished: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << "ms";



    // test graphs
    for(int i = 0; i < graph_group.size(); i++)
    {
        graph_group.get_graph(i).show_info();
    }
    // test graphgroup
    std::vector<std::vector<int>> graphMatrix = graph_group.get_graph_matrix();
    for(int i = 0; i < graphMatrix.size(); i++)
    {
        for(int j = 0; j < graphMatrix[i].size(); j++)
        {
            std::cout << graphMatrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    Schedule schedule(graph_group);
    LOG(INFO) << "Schedule finished: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << "ms";

    // test schedule results

    for(int i = 0; i < graph_group.size(); i++)
    {
        Graph& bb = graph_group.get_graph(i);
        std::cout << "Schedule for block " << bb.name << std::endl;
        for(int j = 0; j < bb.num_node; j++)
        {
            std::cout << "Node " << j << " is scheduled at cycle " << bb.statements[j].sch << std::endl;
        }
    }

    Binding binding(graph_group);
    LOG(INFO) << "Binding finished: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << "ms";


    // // test binding results
    // for(int i = 0; i < graphGroup.size(); i++)
    // {
    //     Graph& block = graphGroup.getGraph(i);
    //     std::cout << "Binding for block " << block.name << std::endl;
    //     for(int j = 0; j < block.num_node; j++)
    //     {
    //         std::cout << "Node " << j << " is binded to resource " << block.source[j] << std::endl;
    //     }
    // }
    // LOG(INFO) << "Binding finished: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << "ms";

    GenRTL genRTL(graph_group, "../result/output.v");
    LOG(INFO) << "GenRTL finished: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << "ms";


}
