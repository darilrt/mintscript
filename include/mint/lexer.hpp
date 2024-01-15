#pragma once

#include <string>
#include "mint/token.hpp"

class Lexer
{
public:
    std::streambuf *streambuf;
    Location location;

    Lexer(std::streambuf *streambuf, const std::string &file);

    Token next_token();

private:
    char peek();

    char next();

    char seek();
};