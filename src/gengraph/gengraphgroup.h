#ifndef GENGRAPH_H
#define GENGRAPH_H

#include <vector>
#include "graphgroup.h" 
#include "parser.h"

class GenGraphGroup
{
public:
    GenGraphGroup(Parser&, GraphGroup&);
    ~GenGraphGroup();

};

#endif