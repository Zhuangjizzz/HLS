#include "schedule.h"

Schedule::Schedule(GraphGroup& graphGroup) {
    for (int i = 0; i < graphGroup.size(); i++)
    {
        Graph& bb = graphGroup.get_graph(i);
        ASAP(bb);
    }
}

void Schedule::ASAP(Graph& bb)
{
    int bbTime = 0;
    std::vector<bool> visited(bb.num_node, false);
    std::vector<float> indegree(bb.num_node, 0);
    std::queue<int> nodes;
    for (int i = 0; i < bb.num_node; i++)
    {
        indegree[i] = bb.statements[i].indegree;
        if (bb.statements[i].indegree == 0)
        {
            nodes.push(i);
            bb.statements[i].sch = bbTime;
        }
    }
    while (!nodes.empty())
    {
        int node = nodes.front();
        nodes.pop();
        if (bb.statements[node].type == OP_TYPE::OP_LOAD || bb.statements[node].type == OP_TYPE::OP_STORE)
            bbTime = bb.statements[node].sch + 2;
        else
            bbTime = bb.statements[node].sch + 1;
        visited[node] = true;
        for (int i = 0; i < bb.num_node; i++)
        {
            if (bb.matrix[node][i] == 1)
            {
                indegree[i]--;
                if (!visited[i] && indegree[i] == 0)
                {
                    nodes.push(i);
                    bb.statements[i].sch = bbTime;
                }
            }
        }
        bb.sch_total = bbTime;
    }
}  