#include "graphgroup.h"

GraphGroup::GraphGroup() {
}


GraphGroup::~GraphGroup() {
}

Graph& GraphGroup::getGraph(int index) {
    return graphs[index];
}

void GraphGroup::addGraph(Graph& g) {
    graphs.push_back(g);
}

int GraphGroup::size() {
    return graphs.size();
}




