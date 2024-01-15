#include <iostream>
#include <fstream>
#include <sstream>

#include "mint/lexer.hpp"

int main()
{
    // std::ifstream stream("examples/test.mint");

    std::stringstream file;
    file << "  test";

    Lexer lexer(file.rdbuf(), "examples/test.mint");

    Token token = lexer.next_token();

    while (token.type != Token::EndFile)
    {
        std::cout << token << std::endl;
        token = lexer.next_token();
    }

    return 0;
}
