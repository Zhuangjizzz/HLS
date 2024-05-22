#include "graph.h"

Graph::Graph(int port, Parser& p) {

    basic_block bb = p.get_basic_blocks()[port];
    this->port = port + 1;      // 预留编号0端口
    this->name = bb.get_label_name();
    this->num_node = bb.get_statements().size();
    this->sch_total = 0;
    set_port2index(p);
    for (int i = 0; i < bb.get_statements().size(); i++) {
        Statement s;
        s.type = bb.get_statements()[i].get_type();
        s.var = bb.get_statements()[i].get_var();
        s.num_oprands = bb.get_statements()[i].get_num_oprands();
        s.var = bb.get_statements()[i].get_var();
        s.sch = 0;
        s.indegree = 0;
        if(s.type == OP_TYPE::OP_PHI) {
            std::vector<std::string> oprands = bb.get_statements()[i].get_oprands();
            for(int j = 0; j < oprands.size(); j++){
                if(j % 2 == 0)
                    s.oprands.push_back(oprands[j]);
                else
                    s.oprands.push_back(get_port_index(oprands[j]));
            }
        }
        else if(s.type == OP_TYPE::OP_BR) {
            if(s.num_oprands == 1) {
                std::vector<std::string> oprands = bb.get_statements()[i].get_oprands();
                s.oprands.push_back(get_port_index(oprands[0]));
                }
            if(s.num_oprands == 3) {
                std::vector<std::string> oprands = bb.get_statements()[i].get_oprands();
                s.oprands.push_back(oprands[0]);
                s.oprands.push_back(get_port_index(oprands[1]));
                s.oprands.push_back(get_port_index(oprands[2]));
            }
        }
        else
        {         
            s.oprands = bb.get_statements()[i].get_oprands();
        }
        this->statements.push_back(s);
    }
    // set_inport(p);
    set_outport(p);
    set_matrix();
    set_indegree();
}

void Graph::set_port2index(Parser &p) {
    for (int i = 0; i < p.get_basic_blocks().size(); i++)
    {
        this->port2index[p.get_basic_blocks()[i].get_label_name()] = std::to_string(i + 1);
    }

}

std::string Graph::get_port_index(std::string port) {
    return this->port2index[port];
}


void Graph::set_outport(Parser &p) {
        Statement s = this->statements[num_node - 1];
        if (s.type == OP_TYPE::OP_BR) {
            if (s.num_oprands == 1) {
                this->outports.push_back(std::stoi(s.oprands[0]));
            }
            if (s.num_oprands == 3) {
                this->outports.push_back(std::stoi(s.oprands[1]));
                this->outports.push_back(std::stoi(s.oprands[2]));
            }
        }
        else {
            if (this->port < p.get_basic_blocks().size()) {
                this->outports.push_back(port + 1);
            }
            else {
                this->outports.push_back(0);
            }
        }
}

void Graph::set_matrix(){
    this->matrix.resize(this->num_node, std::vector<int>(this->num_node, 0));
    for (int i = 0; i < num_node; i++)
    {
        Statement s = this->statements[i];
        for (int j = 0; j < i; j++)
        {
            for(int k = 0; k < s.num_oprands; k++)
            {
                if (s.oprands[k] == this->statements[j].var)
                {
                    set_edge(j, i);
                    break;
                }
            }
        }
        if (s.type == OP_TYPE::OP_BR || s.type == OP_TYPE::OP_RET)
        {
            for (int j = 0; j < i; j++)
            {
                set_edge(j, i);
            }
        }
    }
}

void Graph::set_indegree() {
    for (int i = 0; i < num_node; i++)
    {
        for (int j = 0; j < num_node; j++)
        {
            if (matrix[i][j] == 1)
            {
                statements[j].indegree++;
            }
        }
    }
}

void Graph::show_info()
{
    std::cout << name << " port:" << port <<  std::endl;
    for (int i = 0; i < num_node; i++)
    {
        std::cout << "value: " << statements[i].var << ", type: " << statements[i].type << std::endl;
    }
    // std::cout << "outport: ";
    // for (int i = 0; i < outports.size(); i++)
    // {
    //     std::cout << outports[i] << " ";
    // }
    std::cout << std::endl;
    std::cout << "Adjacency Matrix: " << std::endl;
    for (int i = 0; i < num_node; i++)
    {
        for (int j = 0; j < num_node; j++)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl ;
}

void Graph::set_edge(int from, int to) {
    this->matrix[from][to] = 1;
}

Graph::~Graph() {
    // 释放资源代码
}