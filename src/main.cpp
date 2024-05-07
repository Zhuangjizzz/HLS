// #include"Algorithm.h"
#include "Graph.h"
#include "parser.h"
#include "hls.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage:" << argv[0] << "filename\n";
        return -1;
    }
    //parse
    Parser p = Parser(argv[1]);
    if (p.Parse() != 0)
    {
        std::cout << "parsing error" << p.get_current_line() << std::endl;
        return -1;
    }

    HLS hls = HLS(p);
    hls.run();
    //scheduling
    

    //scheduling
//     Algorithm MtchaCoffee;
//     std::vector<int> resConstraint = { 2, 2, 2 };
//     for (int i = 0; i < data_control_stream.size(); i++)
//     {
//         MtchaCoffee.Min_Latency_with_limited_res(resConstraint, data_control_stream[i]);
//         //MtchaCoffee.ASAP(data_control_stream[i]);
//         MtchaCoffee.showTime(data_control_stream[i]);
//     }
// }

}