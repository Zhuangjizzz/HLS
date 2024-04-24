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
	Graph(basic_block&, std::vector<std::string>);
	void showInfo();
	void setEdge(int, int);
	void delEdge(int, int);
	edge getFirstEdge(int);
	edge getNextEdge(const edge&);
	bool IsEdge(const edge&);
	Graph(const Graph&);
	Graph& operator=(const Graph&);
	~Graph();

public:
	std::string name;							//block名称
	int** matrix;								//指向相邻矩阵的指针，矩阵维度位numnode*numnode
	std::vector<int> mark;		 				//标记顶点是否访问过
	std::vector<int> indegree;					//顶点入度
	std::vector<int> op;						//操作类型
	std::vector<int> sch;						//顶点调度周期
	std::vector<int> reg;						//顶点的寄存器
	std::vector<int> source;					//顶点的硬件计算资源
	std::vector<int> outport;					//跳转目标block编号
	std::vector<std::string> values;			//变量名称
	int numnode, numedge, numberofoutport;
	int jumpto;
};


#endif //Graph_H_

