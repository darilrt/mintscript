#include <iostream>

#include "mint/token.hpp"

int main()
{
    Token token{
        .type = Token::Int,
        .value = "123",
        .location = {
            .line = 1,
            .column = 1,
            .file = "main.mint",
        },
    };

    std::cout << token.to_string() << std::endl;
    return 0;
}
