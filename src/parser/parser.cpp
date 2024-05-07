#include "parser.h"
#include <sstream>
#include "string.h"
basic_block::basic_block(std::string &label)
{
    _label = label;
}

void basic_block::add_statement(statement &s)
{
    _statements.push_back(s);
}

Parser::Parser(const std::string &filename)
    : _filename(filename)
{
}

int Parser::Parse()
{
    std::ifstream inf(_filename);//打开文件
    if (!inf)
        return 1;

    std::string line;
    std::string tok;
    std::istringstream iss;
    int function_parse = 0;
    int parse_error = 0;
    basic_block bb;

    bb.set_name("0"); // set to 0 for default

    if (!inf)
        return 1;

    while (getline(inf, line))  //逐行读取
    {
        _line = line;
        iss.clear();
        iss.str(line);

        if (!(iss >> tok))  //如果从iss输入流中提取失败，则跳过
            continue;
        else
        {
            if (tok == "define")    //LLVM的定义函数语句
            {
                function_parse = parse_function(line);
            }
            else if (function_parse == 0)   //parse_function no error
            {
                if (parse_statement(line, bb) != 0)
                {
                    parse_error = 1;
                    break;
                }
            }
            else
            {
                parse_error = 1;
                break;
            }
        }
    }
    push_back_basic_block(bb);

    return parse_error;
}

int Parser::parse_function(std::string &line)
{
    char *tok;
    std::string key;
    char sep[] = " (,)";    //以空格，左右括号和逗号为分隔符，这些符号不会被读入tok
    int cnt = 0;
    var p;          //读入变量名并检查是否为数组
    int pos;
    int parser_error = 0;

    tok = strtok(&line[0], sep);    //strtok函数返回指向下一个字符串的指针

    while ((tok = strtok(NULL, sep)) != NULL)
    {
        cnt++;
        key = std::string(tok);
        if (key == "int" && cnt < 2)    //第一个是返回值类型
        {
            _ret_type = RET_INT;
        }
        else if (key == "void")
        {
            _ret_type = RET_VOID;
        }
        else if (cnt == 2)  //第二个是函数名
        {
            _function_name = key;
        }
        else if (cnt > 2)   //后面都是
        {
            if (cnt % 2 == 0)   //varname || varname[]
            {
                key = std::string(tok);
                pos = key.find_first_of('[');
                if (pos == std::string::npos)   //未找到[
                {
                    p._name = key;
                    p._array_flag = false;
                }
                else
                {
                    p._name = key.substr(0, pos);   //提取key[0:pos-1]
                    p._array_flag = true;
                }
                _function_params.push_back(p);
            }
        }
        else
        {
            parser_error = 1;
        }
    }
    return parser_error;
}

void basic_block::clear_statements()
{
    _statements.clear();
}

int Parser::parse_statement(std::string &line, basic_block &bb)
{
    char *tok;
    std::string key;
    char sep[] = " (,);";   //分隔符包括空格，左右括号，逗号，分号
    int cnt = 0;
    var p;
    int error = 0;
    statement s;
    int label = 0;

    tok = strtok(&line[0], sep);    //line第一个字符串
    // 1. label
    key = std::string(tok);
    if (key.find(':') != std::string::npos) //标签行
    {
        push_back_basic_block(bb);  //把上一个处理好的bb加入
        bb.clear_statements();      //清空
        std::string name = key.substr(0, key.length() - 1);
        bb.set_name(name);  //获取下一个block的名称
        label = 1;
    }
    else if (key == "store") // store
    {
        s.set_type(OP_STORE);
        s.set_num_oprands(3);   //store有三个操作数
        for (int i = 0; i < 3; ++i)
        {
            tok = strtok(NULL, sep);
            s.add_oprand(tok);  //操作数
        }
    }
    else if (key == "br") // br
    {
        int num_ops = 0;
        s.set_type(OP_BR);
        while ((tok = strtok(NULL, sep)) != NULL)   //br有两种写法，操作数不一定
        {
            num_ops++;
            s.add_oprand(tok);
        }
        s.set_num_oprands(num_ops);
    }
    else if (key == "return") // ret
    {
        int num_ops = 0;
        s.set_type(OP_RET);
        while ((tok = strtok(NULL, sep)) != NULL)
        {
            num_ops++;
            s.add_oprand(tok);
        }
        s.set_num_oprands(num_ops);
    }
    else    //下面这些指令的名称都不是从行首开始的
    {
        char sep2[] = " (,);";
        int num_ops = 0;
        std::vector<std::string> keys;
        keys.push_back(key);    //key是等号左侧变量
        while ((tok = strtok(NULL, sep)) != NULL)
        {
            keys.push_back(tok);//存入等号，以及等号右侧指令和操作数
        }
        if (keys[2] == "phi")
        {
            s.set_var(key);
            s.set_type(OP_PHI);
            s.set_num_oprands(keys.size() - 3); //除了前面三个后面都是
            for (int i = 3; i < keys.size(); ++i)
                s.add_oprand(keys[i]);
        }
        else if (keys[2] == "load")
        {
            s.set_var(key);
            s.set_type(OP_LOAD);
            s.set_num_oprands(keys.size() - 3);
            for (int i = 3; i < keys.size(); ++i)
                s.add_oprand(keys[i]);
        }
        else if (keys.size() == 3) // assign
        {
            s.set_var(key);
            s.set_type(OP_ASSIGN);
            s.set_num_oprands(1);
            s.add_oprand(keys[2]);
        }
        else if (keys[3] == "+")
        {
            s.set_var(key);
            s.set_type(OP_ADD);
            s.set_num_oprands(2);
            s.add_oprand(keys[2]);
            s.add_oprand(keys[4]);
        }
        else if (keys[3] == "-")
        {
            s.set_var(key);
            s.set_type(OP_SUB);
            s.set_num_oprands(2);
            s.add_oprand(keys[2]);
            s.add_oprand(keys[4]);
        }
        else if (keys[3] == "*")
        {
            s.set_var(key);
            s.set_type(OP_MUL);
            s.set_num_oprands(2);
            s.add_oprand(keys[2]);
            s.add_oprand(keys[4]);
        }
        else if (keys[3] == "/")
        {
            s.set_var(key);
            s.set_type(OP_DIV);
            s.set_num_oprands(2);
            s.add_oprand(keys[2]);
            s.add_oprand(keys[4]);
        }
        else if (keys[3] == ">")
        {
            s.set_var(key);
            s.set_type(OP_GT);
            s.set_num_oprands(2);
            s.add_oprand(keys[2]);
            s.add_oprand(keys[4]);
        }
        else if (keys[3] == "<")
        {
            s.set_var(key);
            s.set_type(OP_LT);
            s.set_num_oprands(2);
            s.add_oprand(keys[2]);
            s.add_oprand(keys[4]);
        }
        else if (keys[3] == "==")
        {
            s.set_var(key);
            s.set_type(OP_EQ);
            s.set_num_oprands(2);
            s.add_oprand(keys[2]);
            s.add_oprand(keys[4]);
        }
        else if (keys[3] == ">=")
        {
            s.set_var(key);
            s.set_type(OP_GE);
            s.set_num_oprands(2);
            s.add_oprand(keys[2]);
            s.add_oprand(keys[4]);
        }
        else if (keys[3] == "<=")
        {
            s.set_var(key);
            s.set_type(OP_LE);
            s.set_num_oprands(2);
            s.add_oprand(keys[2]);
            s.add_oprand(keys[4]);
        }
        else
            error = 1;
    }
    if (error == 0 && label == 0)
        bb.add_statement(s);

    return error;
}
