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

    void set_name(std::string& name) { function_name = name; };
    std::string get_name() { return function_name; };
    void set_ret_type(int type) { ret_type = type; };
    int get_ret_type() { return ret_type; };
    void set_params(std::vector<var>& params) { function_params = params; };
    std::vector<var>& get_params() { return function_params; };

    void set_global_vars(std::vector<std::string>& vars) { global_vars = vars; };
    std::vector<std::string>& get_global_vars() { return global_vars; };

    void set_regs_num(int num) { regs_num = num; };
    int get_regs_num() { return regs_num; };

    void add_graph(Graph&);
    Graph& get_graph(int index);
    std::vector<Graph>& get_graphs();

    void set_graph_matrix(std::vector<std::vector<int>>&);
    std::vector<std::vector<int>>& get_graph_matrix();

    int size();

private:
    std::string function_name;
    int ret_type;
    std::vector<var> function_params;
    std::vector<std::string> global_vars;
    int regs_num;
    std::vector<Graph> graphs;
    std::vector<std::vector<int>> graph_matrix;

};

#endif //GRAPH_GROUP_H_