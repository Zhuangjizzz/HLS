#ifndef BINDING_H
#define BINDING_H

#include <vector>
#include "GraphGroup.h"
#include <map>
class Binding
{
public:
    Binding(GraphGroup&);
    ~Binding();

private:
    void bfs(GraphGroup&);
    bool is_global_var(std::string var);
    bool is_param_var(std::string var);
    bool is_local_var(std::string var);
    bool is_constant_var(std::string var);
    void local_binding(Graph&);
    void global_binding(Graph&);
    void left_hand_algorithm(Graph&, std::map<std::string, std::pair<int, int>>&);

    std::vector<std::string> global_vars;
    std::vector<std::string> local_vars;
    std::vector<std::string> param_vars;
    std::vector<std::string> constant_vars;
    int regs_num;

};


// 1. 返回global_vars
// 2. 局部变量绑定的寄存器从0开始

#endif