#include "gengraph.h"


GenGraphGroup::GenGraphGroup(Parser& p, GraphGroup& graphGroup) {

    std::vector<std::string> blockname;
    // 生成图组的代码
    for(int i = 0; i < p.get_basic_blocks().size(); i++)
    {
        blockname.push_back(p.get_basic_blocks()[i].get_label_name());
    }
    for(int i = 0; i < p.get_basic_blocks().size(); i++)
    {
        Graph g = Graph(p.get_basic_blocks()[i],blockname);
        graphGroup.addGraph(g);
    }

}

GenGraphGroup::~GenGraphGroup() {
}