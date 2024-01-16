#pragma once

#include <string>
#include "mint/token.hpp"

class Lexer
{
public:
    std::streambuf *streambuf;
    Location location;

    Lexer() = default;

    Lexer(std::streambuf *streambuf, const std::string &file);

    Token next_token();

private:
    char peek();

    char next();

    char seek();

    Token get_number_token();

    Token get_string_token(char delimiter);

    Token get_identifier_token();

    Token get_keyword_from_identifier(const std::string &identifier);
};