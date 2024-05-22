#include "graphgroup.h"

GraphGroup::GraphGroup() {
}

GraphGroup::~GraphGroup() {
}

Graph& GraphGroup::get_graph(int index) {
    return graphs[index];
}

std::vector<Graph>& GraphGroup::get_graphs() {
    return graphs;
}

void GraphGroup::add_graph(Graph& g) {
    graphs.push_back(g);
}

void GraphGroup::set_graph_matrix(std::vector<std::vector<int>>& matrix) {
    graph_matrix = matrix;
}

std::vector<std::vector<int>>& GraphGroup::get_graph_matrix() {
    return graph_matrix;
}

int GraphGroup::size() {
    return graphs.size();
}




