#include <iostream>
#include <fstream>
#include <sstream>

#include "mint/parser.hpp"

int main()
{
    Parser parser("examples/test.mint");

    Shared<Ast> ast = parser.parse();

    std::cout << ast->to_string() << std::endl;

    return 0;
}
