#ifndef ALGORITHM_H_
#define ALGORITHM_H

#include "graphgroup.h"

class Schedule
{
public:
	Schedule(GraphGroup& g);
	
	void ASAP(Graph&);
	void Min_Latency_with_limited_res(std::vector<int>&, Graph&);	//支持ALU、乘法器、除法器的约束
	void showTime(Graph&);
};

#endif //ALGORITHM_H

