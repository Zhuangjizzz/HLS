#ifndef GRAPH_GROUP_H_
#define GRAPH_GROUP_H_

#include <vector>
#include <string>

#include "Graph.h"

class GraphGroup
{
public:
    GraphGroup();
    ~GraphGroup();

    void addGraph(Graph&);
    Graph& getGraph(int index);
    int size();


private:
    std::vector<Graph> graphs;
};

#endif //GRAPH_GROUP_H_