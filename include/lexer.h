#pragma once

#include "token.h"

#include <string>

class Lexer {
public:
    Lexer(const std::string& source, const std::string& filename="");

    inline char Peek() const { return source[pos]; }

    char Next();

    Token NextToken();

    Token GetKeywordToken(const std::string& value);
    
    Token GetNumberToken();

    Token GetStringToken();

private:
    std::string source;
    size_t pos = 0;
    
    Location location;
};