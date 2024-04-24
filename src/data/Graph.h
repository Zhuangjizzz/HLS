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
	std::string name;							//block����
	int** matrix;								//ָ�����ھ����ָ�룬����ά��λnumnode*numnode
	std::vector<int> mark;		 				//��Ƕ����Ƿ���ʹ�
	std::vector<int> indegree;					//�������
	std::vector<int> op;						//��������
	std::vector<int> sch;						//�����������
	std::vector<int> reg;						//����ļĴ���
	std::vector<int> source;					//�����Ӳ��������Դ
	std::vector<int> outport;					//��תĿ��block���
	std::vector<std::string> values;			//��������
	int numnode, numedge, numberofoutport;
	int jumpto;
};


#endif //Graph_H_

