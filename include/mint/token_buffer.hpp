#pragma once

#include <vector>
#include "mint/lexer.hpp"

class TokenBuffer
{
public:
    Lexer lexer;
    bool ignore_newlines;
    std::vector<Token> tokens;
    std::vector<size_t> states;
    std::vector<bool> ignore_newlines_states;
    size_t index;

    TokenBuffer() : lexer(){};

    TokenBuffer(std::streambuf *streambuf, const std::string &file);

    Token next_token();

    Token peek_token();

    void push_state();

    void pop_state();

    void back_to_state();

    void push_ignore_newlines_state(bool ignore_newlines);

    void pop_ignore_newlines_state();

    void print_tokens();
};