#include <memory>
#include <vector>
#include <string>

#include "mint/error.hpp"
#include "mint/parser.hpp"
#include "mint/token.hpp"
#include "mint/token_buffer.hpp"

std::unique_ptr<Ast> Parser::parse()
{
    return parse_expression();
}

std::unique_ptr<Ast> Parser::parse_expression()
{
    return parse_logical_or();
}

std::unique_ptr<Ast> Parser::parse_logical_or()
{
    std::unique_ptr<Ast> ast = parse_logical_and();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_logical_and()
{
    std::unique_ptr<Ast> ast = parse_bitwise_or();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_bitwise_or()
{
    std::unique_ptr<Ast> ast = parse_bitwise_xor();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_bitwise_xor()
{
    std::unique_ptr<Ast> ast = parse_bitwise_and();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_bitwise_and()
{
    std::unique_ptr<Ast> ast = parse_equality();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_equality()
{
    std::unique_ptr<Ast> ast = parse_comparison();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_comparison()
{
    std::unique_ptr<Ast> ast = parse_shift();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_shift()
{
    std::unique_ptr<Ast> ast = parse_additive();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_additive()
{
    std::unique_ptr<Ast> ast = parse_multiplicative();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_multiplicative()
{
    std::unique_ptr<Ast> ast = parse_unary();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_unary()
{
    std::unique_ptr<Ast> ast = parse_postfix();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_postfix()
{
    std::unique_ptr<Ast> ast = parse_factor();
    return ast;
}

std::unique_ptr<Ast> Parser::parse_factor()
{
    std::unique_ptr<Ast> ast = std::make_unique<Ast>();

    Token token = buffer.next_token();

    switch (token.type)
    {
    case Token::Integer:
        ast->type = Ast::Integer;
        ast->value = new int(std::stoi(token.value));
        break;
    case Token::Float:
        ast->type = Ast::Float;
        ast->value = new float(std::stof(token.value));
        break;
    case Token::String:
        ast->type = Ast::String;
        ast->value = new std::string(token.value);
        break;
    }

    if (ast->type == Ast::None)
    {
        Error::error_in_line("Syntax error: expected literal", token, file);
    }

    return ast;
}