#include "Graph.h"


Graph::Graph(basic_block& block, std::vector<std::string> blockname) : jumpto(0), numedge(0)	//�����blocknameһ��Ҫ��˳��
{
	std::vector<statement> & Vs= block.get_statements();
	numnode = Vs.size();		//��������

	//initialize
	matrix = new int* [numnode];
	for (int i = 0; i < numnode; i++)
	{
		matrix[i] = new int[numnode];
		for (int j = 0; j < numnode; j++)
			matrix[i][j] = 0;
		mark.push_back(0);
		indegree.push_back(0);
		sch.push_back(0);
		reg.push_back(0);
		source.push_back(0);
	}

	//ȷ��outport
	bool hasBR = false;
	for (std::vector<statement>::iterator it = Vs.begin(); it != Vs.end(); ++it)
	{
		//����ҵ���br���͸���brȷ����ת�˿�
		if (it->get_type() == OP_TYPE::OP_BR)
		{
			hasBR = true;
			if (it->get_num_oprands() == 1)	//br label
			{
				numberofoutport = 1;
				for (int i = 0; i < blockname.size(); i++)
				{
					if (blockname[i] == it->get_oprand(0))
						outport.push_back(i);
				}
			}
			else if (it->get_num_oprands() == 3)	//br cond label1 label2
			{
				numberofoutport = 2;
				//���ϲ�ѭ������֤label1�ȱ�push��outport����
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
			else	//brû�����������
			{
				std::cout << "br oprand number error!" << std::endl;
				exit(2);
			}
			break;
		}
	}
	if (!hasBR)	//û��br��ֱ����ת�����ڵ���һ��block����������һ����outport��Ϊ-1
	{
		numberofoutport = 1;
		for (int i = 0; i < blockname.size(); i++)
		{
			if (blockname[i] == block.get_label_name())
				if (i == blockname.size() - 1)
					outport.push_back(-1);
				else
					outport.push_back(i + 1);
		}
	}
	
	//����������ֵ����
	for (int i = 0; i < numnode; i++)
	{
		values.push_back(Vs[i].get_var());
		op.push_back(Vs[i].get_type());
	}
	
	//����������������ϵ
	for (int i = 0; i < numnode; i++)
	{
		statement& s = Vs[i];
		for (int j = 0; j < s.get_num_oprands(); j++)
		{
			for (int k = 0; k < i; k++)
			{
				//�����������֮ǰ��ֵ��λ�ó��ֹ�
				if (s.get_oprand(j) == Vs[k].get_var())
					setEdge(k, i);
			}
		}
		//�����br����returnָ���Ҫ��ǰ���ȫ�����
		if (s.get_type() == OP_BR || s.get_type() == OP_RET)
		{
			for (int k = 0; k < i; k++)
			{
				setEdge(k, i);
			}
		}
	}
}

void Graph::showInfo()
{
	for (int i = 0; i < numnode; i++)
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
	for (int i = 0; i < numnode; i++)
	{
		for (int j = 0; j < numnode; j++)
		{
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl ;
}

void Graph::setEdge(int from, int to)
{
	//���ԭ��û�б�
	if (matrix[from][to] <= 0)
	{
		numedge++;
		indegree[to]++;
	}
	matrix[from][to] = 1;
}

void Graph::delEdge(int from, int to)
{
	//���ԭ���б�
	if (matrix[from][to] > 0)
	{
		numedge--;
		indegree[to]--;
	}
	matrix[from][to] = 0;
}

edge Graph::getFirstEdge(int vertex)
{
	edge myedge;
	myedge.from = vertex;
	for (int i = 0; i < numnode; i++)
	{
		if (matrix[vertex][i] != 0)
		{
			myedge.to = i;
			break;
		}
	}
	return myedge;
}

edge Graph::getNextEdge(const edge& preedge)
{
	edge myedge;
	myedge.from = preedge.from;
	if (preedge.to < numnode)
	{
		for (int i = preedge.to + 1; i < numnode; i++)
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

bool Graph::IsEdge(const edge& e)
{
	if (e.to == -1)
		return false;
	else
		return true;
}

Graph::Graph(const Graph& other)
	: numnode(other.numnode), numedge(other.numedge), numberofoutport(other.numberofoutport), jumpto(other.jumpto)
{

	// Deep copy for mark, indegree, op, sch, reg, source, outport, values vectors
	this->mark = other.mark;
	this->indegree = other.indegree;
	this->op = other.op;
	this->sch = other.sch;
	this->reg = other.reg;
	this->source = other.source;
	this->outport = other.outport;
	this->values = other.values;

	// Deep copy for matrix
	this->matrix = new int* [numnode];
	for (int i = 0; i < numnode; i++) {
		this->matrix[i] = new int[numnode];
		for (int j = 0; j < numnode; j++) {
			this->matrix[i][j] = other.matrix[i][j];
		}
	}
}

Graph& Graph::operator=(const Graph& other) {
	if (this != &other) { // self-assignment check
		// Free current resources
		for (int i = 0; i < numnode; i++) {
			delete[] this->matrix[i];
		}
		delete[] this->matrix;

		// Copy data from other
		numnode = other.numnode;
		numedge = other.numedge;
		numberofoutport = other.numberofoutport;
		jumpto = other.jumpto;

		mark = other.mark;
		indegree = other.indegree;
		op = other.op;
		sch = other.sch;
		reg = other.reg;
		source = other.source;
		outport = other.outport;
		values = other.values;

		// Deep copy for matrix
		this->matrix = new int* [numnode];
		for (int i = 0; i < numnode; i++) {
			this->matrix[i] = new int[numnode];
			for (int j = 0; j < numnode; j++) {
				this->matrix[i][j] = other.matrix[i][j];
			}
		}
	}
	return *this;
}

Graph::~Graph() {
	// Free allocated memory for matrix
	for (int i = 0; i < numnode; i++) {
		delete[] matrix[i];
	}
	delete[] matrix;
}
