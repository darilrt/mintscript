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
    // Program
    // <program> ::= <statement> { "\n" <statement> }

    std::unique_ptr<Ast> parse_program();

    // Statements
    // <statement> ::= <expression>

    std::unique_ptr<Ast> parse_statement();

    // Types
    // <type> ::= "int" | "float" | "string" | "bool"

    std::unique_ptr<Ast> parse_type();

    // Expressions
    // <expr_list> ::= <expression> { "," { <expression> } }
    // <expression> ::= <logical_or>
    // <logical_or> ::= <logical_and> { "||" <logical_and> }
    // <logical_and> ::= <bitwise_or> { "&&" <bitwise_or> }
    // <bitwise_or> ::= <bitwise_xor> { "|" <bitwise_xor> }
    // <bitwise_xor> ::= <bitwise_and> { "^" <bitwise_and> }
    // <bitwise_and> ::= <equality> { "&" <equality> }
    // <comparison> ::= <shift> { ("==" | "!=" | "<" | ">" | "<=" | ">=") <shift> }
    // <shift> ::= <additive> { ("<<" | ">>") <additive> }
    // <additive> ::= <multiplicative> { ("+" | "-") <multiplicative> }
    // <multiplicative> ::= <unary> { ("*" | "/" | "%") <unary> }
    // <unary> ::= ("!" | "~" | "-" | "+" | "++" | "--") <unary> | <postfix>
    // <postfix> ::= <factor> ("++" | "--" | "as" <type>)
    // <factor> ::= <integer> | <float> | <string> | <boolean> | <identifier> | "(" <expression> ")"

    std::unique_ptr<Ast> parse_expr_list();
    std::unique_ptr<Ast> parse_expression(bool ignore_newlines);
    std::unique_ptr<Ast> parse_logical_or();
    std::unique_ptr<Ast> parse_logical_and();
    std::unique_ptr<Ast> parse_bitwise_or();
    std::unique_ptr<Ast> parse_bitwise_xor();
    std::unique_ptr<Ast> parse_bitwise_and();
    std::unique_ptr<Ast> parse_comparison();
    std::unique_ptr<Ast> parse_shift();
    std::unique_ptr<Ast> parse_additive();
    std::unique_ptr<Ast> parse_multiplicative();
    std::unique_ptr<Ast> parse_unary();
    std::unique_ptr<Ast> parse_postfix();
    std::unique_ptr<Ast> parse_factor();
};