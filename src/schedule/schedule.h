#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "graphgroup.h"

class Schedule
{
public:
    Schedule(GraphGroup& g);

    void ASAP(Graph&);

};

#endif //SCHEDULE_H