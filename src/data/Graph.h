#ifndef Graph_H_
#define Graph_H_

#include"parser.h"


class edge
{
public:
	edge(): from(-1), to(-1)
	{
		;
	}
	edge(int f, int t): from(f), to(t)
	{
		;
	}

public:
	int from, to;
};


class Graph
{
public:
	Graph();
	Graph(basic_block&, std::vector<std::string>);
	~Graph();
	void show_info();
	void set_edge(int, int);
	void del_edge(int, int);
	edge get_first_edge(int);
	edge get_next_edge(const edge&);
	bool is_edge(const edge&);
	// Graph(const Graph&);
	// Graph& operator=(const Graph&);
	// ~Graph();

public:
	std::string name;							//block名称
	std::vector<std::vector<int>> matrix;	    //指向相邻矩阵的指针，矩阵维度位numnode*numnode
	std::vector<int> mark;		 				//标记算子是否访问过
	std::vector<int> indegree;					//算子入度
	std::vector<int> op;						//算子操作类型
	std::vector<int> sch;						//算子调度周期
	// std::vector<int> reg;						//算子对应的寄存器
	std::vector<int> source;					//算子使用的硬件资源编号

	std::vector<int> outport;					//跳转目标block编号
	std::vector<std::string> values;			//变量名称
	int num_node, num_edge, num_outport;
	int jumpto;
};


#endif //Graph_H_

