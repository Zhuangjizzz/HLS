#include "genrtl.h"
#include <cmath>
#include <algorithm>
#define tab(t) std::string((t), '\t')
#define bitwidth(width) std::string("["+std::to_string(width)+"-1:0]")
#define state_trans(t) std::string(state_name + " <= " + std::to_string(state_width) + "\'d" + std::to_string(t) + ";")
#define prev_state_trans(t) std::string(prev_state_name + " <= " + std::to_string(state_width) + "\'d" + std::to_string(t) + ";")
#define branch_state_trans(t) std::string(branch_state_name + " <= " + std::to_string(state_width) + "\'d" + std::to_string(t) + ";")
#define sub_state_trans(t) std::string(sub_state_name + " <= " + std::to_string(sub_state_width) + "\'d" + std::to_string(t) + ";")
#define state_value(t) std::string(std::to_string(state_width) + "\'d" + std::to_string(t))
#define sub_state_value(t) std::string(std::to_string(sub_state_width) + "\'d" + std::to_string(t))
#define value(width, t) std::string(std::to_string(width) + "\'d" + std::to_string(t))
#define non_block_assign(var, val) std::string(var + " <= " + val + ";")

GenRTL::GenRTL(GraphGroup& graph_group, std::string output_file) {
    rst_type = false;
    data_width = 32;
    sram_addr_width = 16;
    clk_name = "clk";
    rst_name = rst_type ? "rst_n" : "rst";
    start_sig = "start_sig";
    state_name = "state";
    prev_state_name = "prev_state";
    sub_state_name = "sub_state";
    branch_state_name = "branch_state";
    done_flag_name = "done_flag";

    std::ofstream f(output_file);
    if (!f) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    gen_ports(f, graph_group);
    gen_regs(f,graph_group);
    gen_FSM(f, graph_group);
}




void GenRTL::gen_ports(std::ofstream& f, GraphGroup& graph_group){
    // print in/out ports
    f << "module " << graph_group.get_name() << "(\n";
    f << "\tinput\t" << clk_name << ",\n";
    f << "\tinput\t" << rst_name << ",\n";
    f << "\tinput\t" << start_sig << ",\n";
    // print parameters
    for (int i = 0; i < graph_group.get_params().size(); i++) {
        if(graph_group.get_params()[i]._array_flag)
        {
            std::string var_name = graph_group.get_params()[i]._name;
            f << "\t// sram " << var_name << "\n";
            f << "\tinput\t\t" << bitwidth(data_width) << "\t" << var_name << "_in,\n";
            f << "\toutput\treg\t" << bitwidth(data_width) << "\t" << var_name << "_out,\n";
            f << "\toutput\treg\t\t\t\t" << var_name << "_wr_en,\n";
            f << "\toutput\treg\t\t\t\t" << var_name << "_rd_en,\n";
            f << "\toutput\treg\t" << bitwidth(sram_addr_width) << "\t" << var_name << "_addr,\n";
        }
        else
        {
            std::string var_name = graph_group.get_params()[i]._name;
            f << "\tinput\t\t" << bitwidth(data_width) << "\t" << var_name << ",\n";
        }
    }
    // print result
    if (graph_group.get_ret_type() == RET_TYPE::RET_INT)
    {
        f << "\toutput\treg\t" << bitwidth(data_width) << "\t" << "result,\n";
    }
    // print done_flag
    f << "\toutput\treg\t\t\t\t" << done_flag_name << "\n";
    f << ");\n";
}

void GenRTL::gen_regs(std::ofstream& f, GraphGroup& graph_group){
    // regs
    f << "// regs\n";
    for (int i = 0; i < graph_group.get_regs_num(); i++) {
        f << "reg\t" << bitwidth(data_width) << "reg" << i << ";\n";
    }
    // state registers log(2, state_num + 2)
    f << "// state registers\n";
    int state_num = graph_group.get_graphs().size();
    state_width = ceil(log(state_num + 2)/log(2));
    f << "reg\t" << bitwidth(state_width) << state_name << ";\n";
    f << "reg\t" << bitwidth(state_width) << prev_state_name << ";\n";
    f << "reg\t" << bitwidth(state_width) << branch_state_name << ";\n";
    // sub state
    f << "// sub state\n";
    int maxcycle = 0;
    for (int i = 0; i < graph_group.size(); i++) {
        Graph& bb = graph_group.get_graph(i);
        maxcycle = std::max(maxcycle, bb.sch_total);
    }
    sub_state_width = ceil(log(maxcycle)/log(2));
    f << "reg\t" << bitwidth(sub_state_width) << sub_state_name << ";\n";
}

void GenRTL::gen_FSM(std::ofstream& f, GraphGroup& graph_group){
    // state transition logic
        // state transition logic
    f << "// state transition logic\n";
    int tab = 0;
    f << "always @(posedge clk or negedge " << rst_name << ") begin\n";
    tab++;
    // rst signal
    f << tab(tab) << "if (";
    if (rst_type==false) f << "!";
    f << rst_name << ")" << " begin\n";
    tab++;
    f << tab(tab) << state_name << " <= 0;\n";
    f << tab(tab) << prev_state_name << " <= 0;\n";
    f << tab(tab) << branch_state_name << " <= 0;\n";
    f << tab(tab) << sub_state_name << " <= 0;\n";
    tab--;
    f << tab(tab) << "end\n";

    f << tab(tab) << "else begin\n";
    tab++;
    f << tab(tab) << "case(" << state_name << ")\n";
    tab++;
    f << tab(tab) << state_value(0) << ": begin\n";
    tab++;
    f << tab(tab) << "if (" << start_sig << ") begin\n";
    tab++;
    f << tab(tab) << non_block_assign(state_name, state_value(1)) << "\n";
    f << tab(tab) << non_block_assign(prev_state_name, state_value(0)) << "\n";
    f << tab(tab) << non_block_assign(sub_state_name, sub_state_value(0)) << "\n";
    f << tab(tab) << non_block_assign(done_flag_name, value(1, 0)) << "\n";
    tab--;
    f << tab(tab) << "end\n";
    tab--;
    f << tab(tab) << "end\n";
    for (int i = 0; i < graph_group.size(); i++) {
        // state transition
        f << tab(tab) << state_value(i+1) << ": begin\n";
        tab++;
        Graph& bb = graph_group.get_graph(i);
        int total_cycle = bb.sch_total;
        f << tab(tab) << "if (" << sub_state_name << " == " << sub_state_value(total_cycle) << ") begin\n";
        tab++;
        f << tab(tab) << non_block_assign(state_name, branch_state_name) << "\n";
        f << tab(tab) << non_block_assign(prev_state_name, state_name) << "\n";
        f << tab(tab) << non_block_assign(sub_state_name,sub_state_value(0)) << "\n";
        tab--;
        f << tab(tab) << "end\n";
        f << tab(tab) << "else \n";
        tab++;
        f << tab(tab) << non_block_assign(sub_state_name, sub_state_name + " + 1") << "\n";
        tab--;
        // output 
        f << tab(tab) << "case (" << sub_state_name << ")\n";
        tab++;
        bool ld_flag_a = false, ld_flag_b = false;
        int statement_num_a = 0, statement_num_b = 0;
        for (int j = 0; j < total_cycle; j++) {
            // 1. 找到当前周期的所有语句
            // 2. 找到语句中变量和操作数对应的寄存器
            // 注意：看到load指令的处理
            f << tab(tab) << sub_state_value(j) << ": begin\n";
            tab++;
            if (ld_flag_a == true)
            {
                std::string var = "a_rd_en";
                f << tab(tab) << non_block_assign(var, "1") << "\n";
                f << tab(tab) << non_block_assign(bb.var_reg[bb.statements[statement_num_a].var], "a_in") << "\n";
                ld_flag_a = false;
            }
            if (ld_flag_b == true)
            {
                std::string var = "b_rd_en";
                f << tab(tab) << non_block_assign(var, "1") << "\n";
                f << tab(tab) << non_block_assign(bb.var_reg[bb.statements[statement_num_b].var], "b_in") << "\n";
                ld_flag_b = false;
            }
            for (int k = 0; k < bb.num_node; k++) {
                Statement& s = bb.statements[k];
                if (s.sch != j) continue;
                switch (s.type) {
                    case OP_TYPE::OP_PHI:
                        f << tab(tab) << "// phi\n";
                        f << tab(tab) << "case (" << prev_state_name << ")\n";
                        tab++;
                        for(int l = 1; l < s.num_oprands; l += 2) {
                            f << tab(tab) << state_value(std::stoi(s.oprands[l])) \
                                << ": " << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[l-1]]) << "\n";
                        }
                        tab--;
                        f << tab(tab) << "endcase\n";
                    break;
                    case OP_TYPE::OP_BR:
                        f << tab(tab) << "// branch\n";
                        if(s.num_oprands == 1)
                        {
                            f << tab(tab) << branch_state_name << " <= " << state_value(std::stoi(s.oprands[0])) << ";\n";
                        }
                        if(s.num_oprands == 3)
                        {
                            f << tab(tab) << branch_state_name << " <= " << bb.var_reg[s.oprands[0]] << " ? "\
                                << state_value(std::stoi(s.oprands[1])) << " : " << state_value(std::stoi(s.oprands[2])) << ";\n";
                        }
                    break;
                    case OP_TYPE::OP_ADD:
                        f << tab(tab) << "// add\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[0]] + " + " + bb.var_reg[s.oprands[1]]) << "\n";
                    break;
                    case OP_TYPE::OP_ASSIGN:
                        f << tab(tab) << "// assign\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[0]]) << "\n";
                    break;
                    case OP_TYPE::OP_DIV:
                        f << tab(tab) << "// div\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[0]] + " / " + bb.var_reg[s.oprands[1]]) << "\n";
                    break;
                    case OP_TYPE::OP_EQ:
                        f << tab(tab) << "// eq\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[0]] + " == " + bb.var_reg[s.oprands[1]]) << "\n";
                    break;
                    case OP_TYPE::OP_GE:
                        f << tab(tab) << "// ge\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], "(" + bb.var_reg[s.oprands[0]] + " >= " + "n)") << "\n";
                    break;
                    case OP_TYPE::OP_GT:
                        f << tab(tab) << "// gt\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[0]] + " > " + bb.var_reg[s.oprands[1]]) << "\n";
                    break;
                    case OP_TYPE::OP_LE:
                        f << tab(tab) << "// le\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[0]] + " <= " + bb.var_reg[s.oprands[1]]) << "\n";
                    break;
                    case OP_TYPE::OP_LOAD:
                        // load指令需要两周期,要进行一些记录
                        f << tab(tab) << "// ld\n";
                        if (s.oprands[0] == "a")
                        {
                            // f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[1]] + " ? " + bb.var_reg[s.oprands[2]] + " : " + bb.var_reg[s.oprands[3]]) << "\n";
                            ld_flag_a = true;
                            f << tab(tab) << non_block_assign(s.oprands[0] + "_rd_en", value(1, 1)) << "\n";
                            f << tab(tab) << non_block_assign(s.oprands[0] + "_addr", bb.var_reg[s.oprands[1]]) << "\n";
                            statement_num_a = k;
                        }
                        if (s.oprands[0] == "b")
                        {
                            // f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[1]] + " ? " + bb.var_reg[s.oprands[2]] + " : " + bb.var_reg[s.oprands[3]]) << "\n";
                            ld_flag_b = true;
                            f << tab(tab) << non_block_assign(s.oprands[0] + "_rd_en", value(1, 1)) << "\n";
                            f << tab(tab) << non_block_assign(s.oprands[0] + "_addr", bb.var_reg[s.oprands[1]]) << "\n";
                            statement_num_b = k;
                        }

                    break;
                    case OP_TYPE::OP_LT:
                        f << tab(tab) << "// lt\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[0]] + " < " + bb.var_reg[s.oprands[1]]) << "\n";
                    break;
                    case OP_TYPE::OP_MUL:
                        f << tab(tab) << "// mul\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[0]] + " * " + bb.var_reg[s.oprands[1]]) << "\n";
                    break;
                    case OP_TYPE::OP_RET:
                        //
                    break;
                    case OP_TYPE::OP_STORE:
                        // store指令需要两周期,要进行一些记录
                    break;
                    case OP_TYPE::OP_SUB:
                        f << tab(tab) << "// sub\n";
                        f << tab(tab) << non_block_assign(bb.var_reg[s.var], bb.var_reg[s.oprands[0]] + " - " + bb.var_reg[s.oprands[1]]) << "\n";
                    break;
                    default:
                        break;
                }
            }
            tab--;
            f << tab(tab) << "end\n";

        }
        f << tab(tab) << "default: ;\n";
        tab--;
        f << tab(tab) << "endcase\n";
        tab--;
        f << tab(tab) << "end\n";
    }
    f << tab(tab) << "default: ;\n";
    tab--;
    f << tab(tab) << "endcase\n"; // case state_name end
    tab--;
    f << tab(tab) << "end\n";   // else begin end
    tab--;
    f << tab(tab) << "end\n";   // always end
    f << tab(tab) << "endmodule\n";
}

GenRTL::~GenRTL() {
}