#include <sstream>
#include <vector>
#include <cstdio>

extern "C"
{
    
int hellocpp(char *ss) {
    std::stringstream output;
    std::vector<std::string> hello = { "Hello" };
    hello.push_back(",");
    hello.push_back(" C++ ");
    hello.push_back("world!");
    for (auto &s : hello) {
        // std::cout does't work ATM :(
        output << s;
    }
    output << std::endl;
    sprintf(ss, "%s\n", output.str().c_str());
    return 0;
}
}