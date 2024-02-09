#pragma once

#include <fstream>

#include "cpptypes.hpp"
#include "mint/token_buffer.hpp"
#include "mint/ast.hpp"

class Parser
{
public:
    std::ifstream file;
    TokenBuffer buffer;

    Parser(const std::string &file_name);

    Unique<Ast> parse_program();

    Unique<Ast> parse();

private:
    Unique<Ast> parse_statement();

    Unique<Ast> parse_type_list();

    Unique<Ast> parse_expression(bool ignore_newlines);

    Unique<Ast> parse_expr_list();

    Unique<Ast> parse_instancing();

    Unique<Ast> parse_postfix_access();

    Unique<Ast> parse_factor();

    Unique<Ast> parse_literal();

    Unique<Ast> parse_struct_brace_initilizer();
};