#pragma once

#include <fstream>
#include <array>

#include "cpptypes.hpp"
#include "mint/token_buffer.hpp"
#include "mint/ast.hpp"
#include "mint/error.hpp"

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

    Unique<Ast> parse_type();

    inline Unique<Ast> parse_dotted_type()
    {
        return parse_binary_operator<Token::Dot>(&Parser::parse_generic_type);
    }

    Unique<Ast> parse_generic_type();

    Unique<Ast> parse_list_type();

    Unique<Ast> parse_optional_type();

    Unique<Ast> parse_expression(bool ignore_newlines);

    Unique<Ast> parse_expr_list();

    inline Unique<Ast> parse_logical_and()
    {
        return parse_binary_operator<Token::AmpAmp>(&Parser::parse_logical_or);
    }

    inline Unique<Ast> parse_logical_or()
    {
        return parse_binary_operator<Token::PipePipe>(&Parser::parse_bitwaise_or);
    }

    inline Unique<Ast> parse_bitwaise_or()
    {
        return parse_binary_operator<Token::Pipe>(&Parser::parse_bitwaise_xor);
    }

    inline Unique<Ast> parse_bitwaise_xor()
    {
        return parse_binary_operator<Token::Caret>(&Parser::parse_bitwaise_and);
    }

    inline Unique<Ast> parse_bitwaise_and()
    {
        return parse_binary_operator<Token::Amp>(&Parser::parse_comparison);
    }

    Unique<Ast> parse_comparison();

    inline Unique<Ast> parse_shift()
    {
        return parse_binary_operator<Token::LessLess, Token::GreaterGreater>(&Parser::parse_additive);
    }

    inline Unique<Ast> parse_additive()
    {
        return parse_binary_operator<Token::Plus, Token::Minus>(&Parser::parse_multiplicative);
    }

    inline Unique<Ast> parse_multiplicative()
    {
        return parse_binary_operator<Token::Star, Token::Slash, Token::Percent>(&Parser::parse_unary);
    }

    Unique<Ast> parse_unary();

    Unique<Ast> parse_postfix();

    Unique<Ast> parse_instancing();

    Unique<Ast> parse_postfix_access();

    Unique<Ast> parse_factor();

    Unique<Ast> parse_literal();

    Unique<Ast> parse_dict();

    Unique<Ast> parse_dict_pair();

    Unique<Ast> parse_struct_brace_initilizer();

    Unique<Ast> parse_struct_brace_initilizer_pair();

    template <Token::Type... Args, typename F>
    Unique<Ast> parse_binary_operator(F f)
    {
        Unique<Ast> lhs = (this->*f)();

        if (lhs->type == Ast::None)
        {
            return lhs;
        }

        buffer.push_state();
        Token token = buffer.next_token();

        std::array<Token::Type, sizeof...(Args)> types = {Args...};

        for (auto type : types)
        {
            if (token.type == type)
            {
                Unique<Ast> binary_operator = std::make_unique<Ast>(Ast::BinaryOperator, token);
                binary_operator->children.push_back(std::move(lhs));

                Unique<Ast> rhs = parse_binary_operator<Args...>(f);

                if (rhs->type == Ast::None)
                {
                    Error::error_in_line("Expected expression after binary operator", token, file);
                }

                binary_operator->children.push_back(std::move(rhs));
                buffer.pop_state();
                return binary_operator;
            }
        }

        buffer.back_to_state();
        return lhs;
    }
};