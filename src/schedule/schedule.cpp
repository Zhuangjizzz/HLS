#include "schedule.h"


Schedule::Schedule(GraphGroup& graphGroup) {
	for (int i = 0; i < graphGroup.size(); i++)
	{
		Graph& block = graphGroup.getGraph(i);
		ASAP(block);
	}

}


//ASAP，没有资源约束，所有计算都是一个周期
void Schedule::ASAP(Graph& block)
{
	//init
	int globalT = 0;
	std::vector<bool> visited(block.num_node, false);
	std::vector<float> indegree(block.num_node, 0);
	std::queue<int> nodes;
	for (int i = 0; i < block.num_node; i++)
	{
		for (int j = 0; j < block.num_node; j++)
		{
			indegree[j] += block.matrix[i][j];
		}
	}

	//ASAP, with global Time record
	for (int i = 0; i < indegree.size(); i++)
	{
		if (indegree[i] == 0)
		{
			nodes.push(i);
			block.sch[i] = globalT;
		}
	}
	while (!nodes.empty())
	{
		int node = nodes.front();
		nodes.pop();
		globalT = block.sch[node] + 1;	//假设运算周期数都是1
		visited[node] = true;
		for (int i = 0; i < block.num_node; i++)
		{
			if (block.matrix[node][i] == 1)
			{
				indegree[i]--;
				if (!visited[i] && indegree[i] == 0)
				{
					nodes.push(i);
					block.sch[i] = globalT;
				}
			}
		}
	}
}


void Schedule::Min_Latency_with_limited_res(std::vector<int>& resConstraint, Graph& block)
{
	//resConstraint[0]->ALU, resConstraint[1]->multiplier, resConstraint[2]->dividor
	//init
	int globalT = 0;
	std::vector<bool> visited(block.num_node, false);
	std::vector<float> indegree(block.num_node, 0);
	std::queue<int> nodes;
	for (int i = 0; i < block.num_node; i++)
	{
		for (int j = 0; j < block.num_node; j++)
		{
			indegree[j] += block.matrix[i][j];
		}
	}

	int ALUnum;
	int mulnum;
	int divnum;
	std::vector<int> nodesToBeSched;
	for (int i = 0; i < indegree.size(); i++)
	{
		if (indegree[i] == 0)
		{
			nodes.push(i);
		}
	}
	while (!nodes.empty())
	{
		ALUnum = 0;
		mulnum = 0;
		divnum = 0;
		if(!nodesToBeSched.empty())
			nodesToBeSched.clear();
		//取出待计算集合Si
		while (!nodes.empty() && ALUnum < resConstraint[0] && mulnum < resConstraint[1] && divnum < resConstraint[2])
		{
			int node = nodes.front();
			nodes.pop();
			nodesToBeSched.push_back(node);
			int TYPE = block.op[node];
			if (TYPE == OP_ADD || TYPE == OP_SUB || TYPE == OP_LT || TYPE == OP_GT || TYPE == OP_GE || TYPE == OP_LE || TYPE == OP_EQ)
				ALUnum++;
			else if (TYPE == OP_MUL)
				mulnum++;
			else if (TYPE == OP_DIV)
				divnum++;
		}
		//分配周期
		for (int j = 0; j < nodesToBeSched.size(); j++) {
			int node = nodesToBeSched[j];
			block.sch[node] = globalT;
			visited[node] = true;
			for (int i = 0; i < block.num_node; i++)
			{
				if (block.matrix[node][i] == 1)
				{
					indegree[i]--;
					if (indegree[i] == 0 && visited[i] == false)
					{
						nodes.push(i);
					}
				}
			}
		}
		globalT++;
	}
}


void Schedule::showTime(Graph& block)
{
	//std::cout << block.name << std::endl;
	for (int j = 0; j < block.num_node; j++)
	{
		std::cout << "value: " << block.values[j] << ", T=" << block.sch[j] << std::endl;
	}
	std::cout << std::endl;
}
