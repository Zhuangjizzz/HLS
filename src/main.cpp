#include "Graph.h"
#include "parser.h"


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage:" << argv[0] << "filename\n";
        return -1;
    }
    Parser p(argv[1]);

    if (p.Parse() != 0)
    {
        std::cout << "parsing error" << p.get_current_line() << std::endl;
        return -1;
    }

    std::vector<std::string> blockname;
    std::vector<basic_block>& blocks = p.get_basic_blocks();
    std::vector<Graph> dataControlStream;
    //get blockname
    for (int i = 0; i < blocks.size(); i++)
    {
        blockname.push_back(blocks[i].get_label_name());
    }
    //generate data-control stream
    for (int i = 0; i < blocks.size(); i++)
    {
        Graph g(blocks[i], blockname);
        dataControlStream.push_back(g);
    }

    //check
    for (int i = 0; i < dataControlStream.size(); i++)
    {
        dataControlStream[i].showInfo();
    }
}
