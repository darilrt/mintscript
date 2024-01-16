#pragma once

#include <memory>
#include <fstream>

#include "mint/token_buffer.hpp"
#include "mint/ast.hpp"

class Parser
{
public:
    std::ifstream file;
    TokenBuffer buffer;

    Parser(const std::string &file_name);

    std::unique_ptr<Ast> parse();

private:
    // Expressions

    // <expression> ::= <logical_or>
    std::unique_ptr<Ast> parse_expression();

    // <logical_or> ::= <logical_and> { "||" <logical_and> }
    std::unique_ptr<Ast> parse_logical_or();

    // <logical_and> ::= <bitwise_or> { "&&" <bitwise_or> }
    std::unique_ptr<Ast> parse_logical_and();

    // <bitwise_or> ::= <bitwise_xor> { "|" <bitwise_xor> }
    std::unique_ptr<Ast> parse_bitwise_or();

    // <bitwise_xor> ::= <bitwise_and> { "^" <bitwise_and> }
    std::unique_ptr<Ast> parse_bitwise_xor();

    // <bitwise_and> ::= <equality> { "&" <equality> }
    std::unique_ptr<Ast> parse_bitwise_and();

    // <equality> ::= <comparison> { ("==" | "!=") <comparison> }
    std::unique_ptr<Ast> parse_equality();

    // <comparison> ::= <shift> { ("<" | ">" | "<=" | ">=") <shift> }
    std::unique_ptr<Ast> parse_comparison();

    // <shift> ::= <additive> { ("<<" | ">>") <additive> }
    std::unique_ptr<Ast> parse_shift();

    // <additive> ::= <multiplicative> { ("+" | "-") <multiplicative> }
    std::unique_ptr<Ast> parse_additive();

    // <multiplicative> ::= <unary> { ("*" | "/" | "%") <unary> }
    std::unique_ptr<Ast> parse_multiplicative();

    // <unary> ::= ("!" | "~" | "-" | "+") <unary> | <postfix>
    std::unique_ptr<Ast> parse_unary();

    // <prefix> ::= ("++" | "--") <prefix> | <unary>
    std::unique_ptr<Ast> parse_prefix();

    // <postfix> ::= <factor> { ("++" | "--") }
    std::unique_ptr<Ast> parse_postfix();

    // <factor> ::= <integer> | <float>
    std::unique_ptr<Ast> parse_factor();
};