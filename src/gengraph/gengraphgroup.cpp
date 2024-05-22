#include "gengraphgroup.h"
#include "parser.h"

GenGraphGroup::GenGraphGroup(Parser& p, GraphGroup& graph_group) {
    graph_group.set_name(p.get_function_name());
    graph_group.set_ret_type(p.get_ret_type());
    graph_group.set_params(p.get_function_params());


    // 生成graphGroup中的每个graph
    for (int i = 0; i < p.get_basic_blocks().size(); i++) {
        Graph g(i, p);
        graph_group.add_graph(g);
    }

    // 生成graph之间的依赖关系,根据graphGroup中的每个graph的port和outport来确定
    std::vector<std::vector<int>> matrix(graph_group.size(), std::vector<int>(graph_group.size(), 0));
    for (const auto& graph : graph_group.get_graphs()){
        for (int outport : graph.outports) {
            if (outport == 0) continue;
            matrix[graph.port-1][outport-1] = 1;  // 设置连接
        }
    }
    graph_group.set_graph_matrix(matrix);
}

GenGraphGroup::~GenGraphGroup() {
    // 释放资源代码
}