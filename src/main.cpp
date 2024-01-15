#include <iostream>
#include <fstream>
#include <sstream>

#include "mint/token_buffer.hpp"

int main()
{
    // std::ifstream stream("examples/test.mint");

    std::stringstream file;
    file << "10 + \n 10\n";

    TokenBuffer buffer(file.rdbuf(), "examples/test.mint");

    buffer.push_ignore_newlines_state(true);
    std::cout << buffer.next_token() << std::endl;
    std::cout << buffer.next_token() << std::endl;
    std::cout << buffer.next_token() << std::endl;
    buffer.pop_ignore_newlines_state();
    std::cout << buffer.next_token() << std::endl;

    return 0;
}
