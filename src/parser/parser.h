#ifndef PARSER_H_
#define PARSER_H_

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include<queue>

enum OP_TYPE
{
    OP_ASSIGN,  //0
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LOAD,
    OP_STORE,
    OP_BR,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
    OP_EQ,
    OP_PHI,
    OP_RET      //14
};

enum RET_TYPE
{
    RET_VOID,
    RET_INT
};

class statement     //一行
{
    public:
       statement() {};
       ~statement() {};

       int get_type() {return _type;}
       void set_type(int type) {_type = type;}
       int get_num_oprands() {return _num_oprands;}
       void set_num_oprands(int t) {_num_oprands = t;}
       std::string & get_oprand(int num) {return _oprands[num];}
       void add_oprand(const std::string & oprand) {_oprands.push_back(oprand);}
       void set_var(const std::string & var) {_var = var;}
       const std::string get_var() {return _var;}
    
    private:
        std::string _line;
        int _type;              //运算类型
        int _num_oprands;       //操作数个数
        std::string _var;                   //左值
        std::vector<std::string> _oprands;  //操作数
};

struct var
{
    public:
    std::string _name;
    bool _array_flag; // = 0 for non-array, >= 1 for array
};

class basic_block
{
    public:
    basic_block() {}
    basic_block(std::string & label);
    void set_name(const std::string & n) {_label = n;}
    void add_statement(statement & s);
    void clear_statements();
    std::vector<statement> & get_statements() {return _statements;}
    std::string get_label_name() {return _label;}

    private:
    std::vector<statement> _statements;
    std::string _label;     //block name
};

class Parser
{
    public:
        Parser(const std::string & filename);
        int Parse();
        std::vector<basic_block> & get_basic_blocks() {return _blocks;}
        std::string & get_function_name() {return _function_name;}
        std::vector<var> & get_function_params() {return _function_params;}
        int get_ret_type() {return _ret_type;}
        std::string & get_current_line() {return _line;}

    private:
        int parse_function(std::string & line);
        int parse_statement(std::string & line, basic_block & bb);
        void push_back_basic_block(basic_block & bb) {_blocks.push_back(bb);}


    private:
        std::string _filename;
        std::vector<basic_block> _blocks;
        std::string _function_name; //函数名称
        std::vector<var> _function_params;  //函数括号内的变量
        int _ret_type;      //函数返回值类型
        std::string _line;
};


#endif
