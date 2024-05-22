// Application.h
#ifndef HLS_H
#define HLS_H

#include "parser.h"
#include "graphgroup.h"
class HLS {
public:
    HLS(Parser& parser);
    ~HLS();

    void run();

private:
    Parser& parser;
    GraphGroup graph_group;
};

#endif
