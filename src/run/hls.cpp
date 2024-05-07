// HLS.cpp
#include "hls.h"
#include "gengraph.h"
#include "schedule.h"
#include <iostream>

HLS::HLS(Parser& parser) : parser(parser){
}

HLS::~HLS() {
    // 释放资源代码
}

void HLS::run() {
    std::cout << "程序启动..." << std::endl;

    GenGraphGroup genGraphGroup(parser, graphGroup);

    // test graphs
    for(int i = 0; i < graphGroup.size(); i++)
    {
        graphGroup.getGraph(i).show_info();
    }

    Schedule schedule(graphGroup);

    // Binding binding(graphGroup);

    // GenRTL genRTL(graphGroup);




    


    // 这里添加程序的主要执行逻辑
    std::cout << "程序结束。" << std::endl;
}
