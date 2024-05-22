#include "binding.h"
#include <map>
#include <set>
#include <cctype>
#include <algorithm>
Binding::Binding(GraphGroup& graph_group) {
    // 1. 找到每个BB的全局变量、常量、函数参数、局部变量
    bfs(graph_group);
    graph_group.set_global_vars(global_vars);
    regs_num = global_vars.size();
    // 2. 对局部变量，进行生命周期分析，绑定到局部寄存器
    for (int i = 0; i < graph_group.size(); i++)
    {
        Graph& bb = graph_group.get_graph(i);
        local_binding(bb);
    }
    graph_group.set_regs_num(regs_num);
    // 3. 对全局变量，直接绑定到全局寄存器
    for (int i = 0; i < graph_group.size(); i++)
    {
        Graph& bb = graph_group.get_graph(i);
        global_binding(bb);
    }
    
}

//
void Binding::bfs(GraphGroup& graph_group) {
    // 1. 找到每个BB的全局变量、常量、函数参数、局部变量
    // 1.1 找到函数参数变量
    for (auto& var: graph_group.get_params()) {
        param_vars.push_back(var._name);
    }
    // 1.2 找到全局变量、常量、局部变量
    std::map<std::string, std::set<int>> vars_appear_cnt; // 统计每个变量在哪些BB中出现
    for (auto& graph: graph_group.get_graphs()) {
        for (auto& statement: graph.statements){
            if(statement.var != "")
                vars_appear_cnt[statement.var].insert(graph.port);
            if(statement.type == OP_TYPE::OP_BR)
            {
                if(statement.oprands.size() == 1)
                    continue;
                if(statement.oprands.size() == 3)
                {
                    vars_appear_cnt[statement.oprands[0]].insert(graph.port);
                }
            }
            else if(statement.type == OP_TYPE::OP_PHI)
            {
                vars_appear_cnt[statement.oprands[0]].insert(graph.port);
                vars_appear_cnt[statement.oprands[2]].insert(graph.port);
            }
            else if(statement.type == OP_TYPE::OP_LOAD)
            {
                vars_appear_cnt[statement.oprands[1]].insert(graph.port);
            }
            else
            {
                for(auto& oprand: statement.oprands)
                    vars_appear_cnt[oprand].insert(graph.port);
            }
        }
    }
    // 对出现在多个BB中的变量，视为全局变量
    for (auto& var: vars_appear_cnt) {
        if (var.second.size() > 1) {
            if (std::isdigit(var.first[0]))
                constant_vars.push_back(var.first);
            else if (!is_param_var(var.first))
                global_vars.push_back(var.first);
        }
        if (var.second.size() == 1) {
            if (std::isdigit(var.first[0]))
                constant_vars.push_back(var.first);
            else if (!is_param_var(var.first))
                local_vars.push_back(var.first);
    }
    }
    // 对每个BB，统计其全局变量
    for (auto& graph: graph_group.get_graphs()) {
        for (auto& statement: graph.statements) {
            if (is_global_var(statement.var))
            {
                graph.var_reg.insert(std::make_pair(statement.var, "-1"));
            }
            else if (!is_param_var(statement.var))
            {
                graph.var_reg.insert(std::make_pair(statement.var, "-1"));
            }
            for (auto& oprand: statement.oprands) {
            if (is_global_var(oprand)) 
            {
                graph.var_reg.insert(std::make_pair(oprand, "-1"));
            }
            else if (!(is_param_var(oprand) || std::isdigit(oprand[0])))
            {
                graph.var_reg.insert(std::make_pair(oprand, "-1"));
            }
            if(is_constant_var(oprand))
            {
                graph.var_reg.insert(std::make_pair(oprand, oprand));
            }
            }
        }
    }
}

bool Binding::is_global_var(std::string var) {
    return ((std::find(global_vars.begin(), global_vars.end(), var)) != global_vars.end());
}
bool Binding::is_param_var(std::string var) {
    return ((std::find(param_vars.begin(), param_vars.end(), var)) != param_vars.end());
}
bool Binding::is_local_var(std::string var) {
    return ((std::find(local_vars.begin(), local_vars.end(), var)) != local_vars.end());
}

bool Binding::is_constant_var(std::string var) {
    return ((std::find(constant_vars.begin(), constant_vars.end(), var)) != constant_vars.end());
}


// 2. 对局部变量，每个BB进行生命周期分析，绑定到局部寄存器
void Binding::local_binding(Graph& bb) {
    for (auto& var: local_vars) {
        bb.vars_life.insert(std::make_pair(var, std::make_pair(-1, -1)));
    }
    for (int i = bb.sch_total-1; i >= 0; i--) {
        for(auto& statement : bb.statements){
            if(statement.sch == i){
                if(is_local_var(statement.var)){
                    bb.vars_life[statement.var].first = i + 1;
                }
                for(auto& oprand: statement.oprands){
                    if(is_local_var(oprand) && bb.vars_life[oprand].second == -1){
                        bb.vars_life[oprand].second = i;
                    }
                }
            }
            else
                continue;
        }
    }
    // 剔除生命周期为0的变量
    for(auto it = bb.vars_life.begin(); it != bb.vars_life.end();){
        if(it->second.first == -1 && it->second.second == -1)
            it = bb.vars_life.erase(it);
        else
            it++;
    }
    left_hand_algorithm(bb, bb.vars_life);
}

void Binding::left_hand_algorithm(Graph& bb, std::map<std::string, std::pair<int, int>>& vars_life) {
    // bb.local_var_reg
    // 1. 将 map 转为 vector,按照生命周期的开始时间排序
    std::vector<std::pair<std::string, std::pair<int, int>>> vars_vector(vars_life.begin(), vars_life.end());

    std::sort(vars_vector.begin(), vars_vector.end(),
        [](const std::pair<std::string, std::pair<int, int>>& a,
           const std::pair<std::string, std::pair<int, int>>& b) {
            return a.second.first < b.second.first;
        });
    // 采用左边算法
    int reg = 0;
    while (!vars_vector.empty())
    {
        std::pair<std::string, std::pair<int, int>> var = vars_vector[0];
        bb.var_reg[var.first] = "reg" + std::to_string(reg + global_vars.size());
        regs_num++;
        int right_edge = var.second.second;
        vars_vector.erase(vars_vector.begin());
        for (int i = 0; i < vars_vector.size(); i++)
        {
            if (vars_vector[i].second.first > right_edge)
            {
                std::pair<std::string, std::pair<int, int>> var = vars_vector[i];
                bb.var_reg[var.first] = "reg" + std::to_string(reg + global_vars.size());
                right_edge = var.second.second;
                vars_vector.erase(vars_vector.begin() + i);
                i--;
            }
        }
        reg++;
    }
}
// 3. 对全局变量，直接绑定到全局寄存器
void Binding::global_binding(Graph& bb) {
    for(auto& statement: bb.statements){
        if(is_global_var(statement.var)){
            int index = std::distance(global_vars.begin(), std::find(global_vars.begin(), global_vars.end(), statement.var));
            bb.var_reg[statement.var] = "reg" + std::to_string(index);
        }
        for(auto& oprand: statement.oprands){
            if(is_global_var(oprand)){
                int index = std::distance(global_vars.begin(), std::find(global_vars.begin(), global_vars.end(), oprand));
                bb.var_reg[oprand] = "reg" + std::to_string(index);
            }
        }
    }
}
Binding::~Binding() {

}