#include <iostream>
#include <fstream>
#include <sstream>

#include "mint/parser.hpp"

int main()
{
    Parser parser("examples/test.mint");

    std::unique_ptr<Ast> ast = parser.parse();

    std::cout << ast->to_string() << std::endl;

    return 0;
}
