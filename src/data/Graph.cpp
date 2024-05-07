#include "Graph.h"



Graph::Graph(basic_block& block, std::vector<std::string> blockname) : jumpto(0), num_edge(0)	//这里的blockname一定要按顺序
{
	std::vector<statement> & Vs= block.get_statements();
	num_node = Vs.size();		//运算数量

	//initialize
	name = block.get_label_name();
	matrix.resize(num_node, std::vector<int>(num_node, 0));
	mark.resize(num_node, 0);
	indegree.resize(num_node, 0);
	sch.resize(num_node, 0);
	// reg.resize(num_node, 0);
	source.resize(num_node, 0);

	//确定outport
	bool hasBR = false;
	for (std::vector<statement>::iterator it = Vs.begin(); it != Vs.end(); ++it)
	{
		//如果找到了br，就根据br确定跳转端口
		if (it->get_type() == OP_TYPE::OP_BR)
		{
			hasBR = true;
			if (it->get_num_oprands() == 1)	//br label
			{
				num_outport = 1;
				for (int i = 0; i < blockname.size(); i++)
				{
					if (blockname[i] == it->get_oprand(0))
						outport.push_back(i);
				}
			}
			else if (it->get_num_oprands() == 3)	//br cond label1 label2
			{
				num_outport = 2;
				//不合并循环，保证label1先被push到outport里面
				for (int i = 0; i < blockname.size(); i++)
				{
					if (blockname[i] == it->get_oprand(1))
						outport.push_back(i);
				}
				for (int i = 0; i < blockname.size(); i++)
				{
					if (blockname[i] == it->get_oprand(2))
						outport.push_back(i);
				}
			}
			else	//br没有其它情况了
			{
				std::cout << "br oprand number error!" << std::endl;
				exit(2);
			}
			break;
		}
	}
	if (!hasBR)	//没有br，直接跳转到紧邻的下一个block；如果是最后一个，outport设为-1
	{
		num_outport = 1;
		for (int i = 0; i < blockname.size(); i++)
		{
			if (blockname[i] == block.get_label_name())
				if (i == blockname.size() - 1)
					outport.push_back(-1);
				else
					outport.push_back(i + 1);
		}
	}
	
	//解析运算左值变量
	for (int i = 0; i < num_node; i++)
	{
		values.push_back(Vs[i].get_var());
		op.push_back(Vs[i].get_type());
	}
	
	//解析操作数依赖关系
	for (int i = 0; i < num_node; i++)
	{
		statement& s = Vs[i];
		for (int j = 0; j < s.get_num_oprands(); j++)
		{
			for (int k = 0; k < i; k++)
			{
				//如果操作数在之前左值的位置出现过
				if (s.get_oprand(j) == Vs[k].get_var())
					set_edge(k, i);
			}
		}
		//如果是br或者return指令，需要等前面的全部完成
		if (s.get_type() == OP_BR || s.get_type() == OP_RET)
		{
			for (int k = 0; k < i; k++)
			{
				set_edge(k, i);
			}
		}
	}
}

void Graph::show_info()
{
	std::cout << name << std::endl;
	for (int i = 0; i < num_node; i++)
	{
		std::cout << "value: " << values[i] << ", type: " << op[i] << std::endl;
	}
	std::cout << "outport: ";
	for (int i = 0; i < outport.size(); i++)
	{
		std::cout << outport[i] << " ";
	}
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

void Graph::set_edge(int from, int to)
{
	//如果原来没有边
	if (matrix[from][to] <= 0)
	{
		num_edge++;
		indegree[to]++;
	}
	matrix[from][to] = 1;
}

void Graph::del_edge(int from, int to)
{
	//如果原来有边
	if (matrix[from][to] > 0)
	{
		num_edge--;
		indegree[to]--;
	}
	matrix[from][to] = 0;
}

edge Graph::get_first_edge(int vertex)
{
	edge myedge;
	myedge.from = vertex;
	for (int i = 0; i < num_node; i++)
	{
		if (matrix[vertex][i] != 0)
		{
			myedge.to = i;
			break;
		}
	}
	return myedge;
}

edge Graph::get_next_edge(const edge& preedge)
{
	edge myedge;
	myedge.from = preedge.from;
	if (preedge.to < num_node)
	{
		for (int i = preedge.to + 1; i < num_node; i++)
		{
			if (matrix[preedge.from][i] != 0)
			{
				myedge.to = i;
				break;
			}
		}
	}
	return myedge;
}

bool Graph::is_edge(const edge& e)
{
	if (e.to == -1)
		return false;
	else
		return true;
}

// Graph::Graph(const Graph& other)
// 	: num_node(other.num_node), num_edge(other.num_edge), num_outport(other.num_outport), jumpto(other.jumpto)
// {

// 	// Deep copy for mark, indegree, op, sch, reg, source, outport, values vectors
// 	this->mark = other.mark;
// 	this->indegree = other.indegree;
// 	this->op = other.op;
// 	this->sch = other.sch;
// 	this->reg = other.reg;
// 	this->source = other.source;
// 	this->outport = other.outport;
// 	this->values = other.values;

// 	// Deep copy for matrix
// 	this->matrix = new int* [num_node];
// 	for (int i = 0; i < num_node; i++) {
// 		this->matrix[i] = new int[num_node];
// 		for (int j = 0; j < num_node; j++) {
// 			this->matrix[i][j] = other.matrix[i][j];
// 		}
// 	}
// }

// Graph& Graph::operator=(const Graph& other) {
// 	if (this != &other) { // self-assignment check
// 		// Free current resources
// 		for (int i = 0; i < num_node; i++) {
// 			delete[] this->matrix[i];
// 		}
// 		delete[] this->matrix;

// 		// Copy data from other
// 		num_node = other.num_node;
// 		num_edge = other.num_edge;
// 		num_outport = other.num_outport;
// 		jumpto = other.jumpto;

// 		mark = other.mark;
// 		indegree = other.indegree;
// 		op = other.op;
// 		sch = other.sch;
// 		reg = other.reg;
// 		source = other.source;
// 		outport = other.outport;
// 		values = other.values;

// 		// Deep copy for matrix
// 		this->matrix = new int* [num_node];
// 		for (int i = 0; i < num_node; i++) {
// 			this->matrix[i] = new int[num_node];
// 			for (int j = 0; j < num_node; j++) {
// 				this->matrix[i][j] = other.matrix[i][j];
// 			}
// 		}
// 	}
// 	return *this;
// }

// Graph::~Graph() {
// 	// Free allocated memory for matrix
// 	for (int i = 0; i < num_node; i++) {
// 		delete[] matrix[i];
// 	}
// 	delete[] matrix;
// }
