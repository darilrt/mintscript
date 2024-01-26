#include <memory>
#include <vector>
#include <string>

#include "mint/error.hpp"
#include "mint/parser.hpp"
#include "mint/token.hpp"
#include "mint/token_buffer.hpp"

Unique<Ast> Parser::parse_expression(bool ignore_newlines)
{
    return parse_literal();
}

Unique<Ast> Parser::parse_expr_list()
{
    Unique<Ast> expr_list = std::make_unique<Ast>(Ast::ExprList);

    Token token = buffer.peek_token();
    do
    {
        Unique<Ast> expr = parse_expression(false);

        if (expr->type == Ast::None)
        {
            break;
        }

        expr_list->children.push_back(std::move(expr));

        buffer.push_state();
        token = buffer.next_token();

        if (token.type == Token::Comma)
        {
            buffer.pop_state();
            continue;
        }

        buffer.back_to_state();
        break;

    } while (true);

    return expr_list;
}

Unique<Ast> Parser::parse_literal()
{
    Token token = buffer.peek_token();

    switch (token.type)
    {
    case Token::Integer:
    {
        buffer.next_token();
        return std::make_unique<Ast>(Ast::Integer, token);
    }

    case Token::Float:
    {
        buffer.next_token();
        return std::make_unique<Ast>(Ast::Float, token);
    }

    case Token::String:
    {
        buffer.next_token();
        return std::make_unique<Ast>(Ast::String, token);
    }

    case Token::True:
    case Token::False:
    {
        buffer.next_token();
        return std::make_unique<Ast>(Ast::Boolean, token);
    }

    case Token::Null:
    {
        buffer.next_token();
        return std::make_unique<Ast>(Ast::Null, token);
    }

    case Token::LBracket:
    {
        buffer.next_token();
        Unique<Ast> array = std::make_unique<Ast>(Ast::Array);
        array->children.push_back(parse_expr_list());
        token = buffer.next_token();
        if (token.type != Token::RBracket)
        {
            Error::error_in_line("Expected ']'", token, file);
        }
        return array;
    }

    case Token::LBrace:
    {
        buffer.next_token();
        // Unique<Ast> dict = std::make_unique<Ast>(Ast::Dict);
        // dict->children.push_back(parse_dict_list());
        token = buffer.next_token();
        if (token.type != Token::RBrace)
        {
            Error::error_in_line("Expected '}'", token, file);
        }
        // return dict;
    }

    default:
        break;
    }

    return std::make_unique<Ast>(Ast::None);
}