#ifndef Graph_H
#define Graph_H

#include "parser.h"
#include <set>
#include <map>
class Statement
{
public:
    int type;
    int num_oprands;
    std::string var;
    std::vector<std::string> oprands;
    int sch;                            //算子调度周期
    int indegree;                       //算子入度
};


class Graph
{
public:
    Graph(int port, Parser& p);
    ~Graph();

    int port, num_node;                 //算子端口号，算子节点数
    std::string name;
    int sch_total;                      //算子总调度周期
    // std::vector<int> inports;
    std::vector<int> outports;


    std::vector<std::vector<int>> matrix;	    //算子的邻接矩阵
    std::vector<Statement> statements;	        //算子的语句

    // std::map<std::string, int> global_var_reg;             //全局变量及其寄存器
    // std::map<std::string, int> local_var_reg;              //局部变量及其寄存器
    std::map<std::string, std::pair<int, int>> vars_life;   //局部变量生命周期

    std::map<std::string, std::string> var_reg;                    //变量及其寄存器
    // 考虑常数变量寄存器存自身，重写寄存器绑定

    void show_info();
protected:
    void set_edge(int from, int to);
    void set_matrix();
    void set_indegree();
    // void set_inport(Parser& p);
    void set_outport(Parser& p);
    void set_port2index(Parser& p);
    std::string get_port_index(std::string port);
private:
    std::map<std::string, std::string> port2index;


};



#endif // Graph_H