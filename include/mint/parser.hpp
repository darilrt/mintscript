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

    Unique<Ast> parse();

private:
    // Program

    Unique<Ast> parse_program();

    // Statements

    Unique<Ast> parse_statement();

    // Types

    Unique<Ast> parse_type();

    // (* Expressions *)
    // expression = logical_or;
    // expr_list = expression, { ',', expression };
    // logical_or = logical_and, { '||', logical_and };
    // logical_and = bitwaise_or, { '&&', bitwaise_or };
    // bitwaise_or = bitwaise_xor, { '|', bitwaise_xor };
    // bitwaise_xor = bitwaise_and, { '^', bitwaise_and };
    // bitwaise_and = comparison, { '&', comparison };
    // comparison = shift, { ( '==' | '!=' | '<' | '>' | '<=' | '>=' ), shift };
    // shift = additive, { ( '<<' | '>>' ), additive };
    // additive = multiplicative, { ( '+' | '-' ), multiplicative };
    // multiplicative = unary, { ( '*' | '/' | '%' ), unary };
    // unary = ( '!' | '~' | '-' | '+' | '++' | '--' ), unary | postfix;
    // postfix = factor, { ( '++' | '--' | 'as', type ) };
    // factor = literal | instancing | '(', expression, ')';
    // instancing = access, [struct_brace_initilizer];
    // access = postfix_access, { '.', postfix_access };
    // postfix_access = ident, { ( '(', expr_list, ')' | '[', ( expr_list | type_list ), ']' ) };
    // literal = integer | float | string | 'true' | 'false' | 'null' | array | dict;
    // array = '[', expr_list, ']';
    // dict_pair = expression, ':', expression;
    // dict_list = dict_pair, { ',', dict_pair };
    // dict = '{', dict_list, '}';
    // struct_brace_initilizer = '{', struct_list, '}';
    // struct_list = struct_pair, { ',', struct_pair };
    // struct_pair = '.', ident, '=', expression;

    Unique<Ast> parse_expression(bool ignore_newlines);
    Unique<Ast> parse_expr_list();
    Unique<Ast> parse_logical_or();
    Unique<Ast> parse_logical_and();
    Unique<Ast> parse_bitwaise_or();
    Unique<Ast> parse_bitwaise_xor();
    Unique<Ast> parse_bitwaise_and();
    Unique<Ast> parse_comparison();
    Unique<Ast> parse_shift();
    Unique<Ast> parse_additive();
    Unique<Ast> parse_multiplicative();
    Unique<Ast> parse_unary();
    Unique<Ast> parse_postfix();
    Unique<Ast> parse_factor();
    Unique<Ast> parse_instancing();
    Unique<Ast> parse_access();
    Unique<Ast> parse_postfix_access();
    Unique<Ast> parse_literal();
    Unique<Ast> parse_array();
    Unique<Ast> parse_dict_pair();
    Unique<Ast> parse_dict_list();
    Unique<Ast> parse_dict();
    Unique<Ast> parse_struct_brace_initilizer();
    Unique<Ast> parse_struct_list();
    Unique<Ast> parse_struct_pair();
};