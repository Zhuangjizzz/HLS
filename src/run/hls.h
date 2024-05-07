// Application.h
#ifndef HLS_H
#define HLS_H

#include "parser/Parser.h"
#include "data/GraphGroup.h"

class HLS {
public:
    HLS(Parser& parser);
    ~HLS();

    void run();

private:
    Parser& parser;
    GraphGroup graphGroup;
};

#endif
