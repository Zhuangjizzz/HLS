#ifndef GENRTL_H
#define GENRTL_H

#include <vector>
#include "graphgroup.h"

class GenRTL
{
public:
    GenRTL(GraphGroup&, std::string);
    ~GenRTL();
private:
    bool rst_type;
    int data_width;
    int sram_addr_width;
    std::string clk_name;
    std::string rst_name;
    std::string start_sig;
    std::string state_name;
    std::string prev_state_name;
    std::string sub_state_name;
    std::string branch_state_name;
    std::string done_flag_name;

    int state_width;
    int sub_state_width;
protected:
    void gen_ports(std::ofstream&, GraphGroup&);
    void gen_regs(std::ofstream&, GraphGroup&);
    void gen_FSM(std::ofstream&, GraphGroup&);
};


#endif