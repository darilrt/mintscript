#include <memory>
#include <vector>
#include <string>

#include "mint/error.hpp"
#include "mint/parser.hpp"
#include "mint/token.hpp"
#include "mint/token_buffer.hpp"

Unique<Ast> Parser::parse_expression(bool ignore_newlines)
{
    buffer.push_ignore_newlines_state(ignore_newlines);
    Unique<Ast> expression = parse_instancing();
    buffer.pop_ignore_newlines_state();

    return expression;
}

Unique<Ast> Parser::parse_expr_list()
{
    Unique<Ast> expr_list = std::make_unique<Ast>(Ast::ExprList);

    buffer.push_ignore_newlines_state(true);
    do
    {
        Unique<Ast> expr = parse_expression(true);

        if (expr->type == Ast::None)
        {
            break;
        }

        expr_list->children.push_back(std::move(expr));

        buffer.push_state();
        Token token = buffer.next_token();

        if (token.type == Token::Comma)
        {
            buffer.pop_state();
            continue;
        }

        buffer.back_to_state();
        break;

    } while (true);
    buffer.pop_ignore_newlines_state();

    return expr_list;
}

Unique<Ast> Parser::parse_instancing()
{
    Unique<Ast> to_instance = parse_postfix_access();

    Unique<Ast> instancing = parse_struct_brace_initilizer();

    if (instancing->type == Ast::None)
    {
        return to_instance;
    }

    Unique<Ast> instantiation = std::make_unique<Ast>(Ast::Instantiation);

    instantiation->children.push_back(std::move(to_instance));
    instantiation->children.push_back(std::move(instancing));

    return instantiation;
}

Unique<Ast> Parser::parse_postfix_access()
{
    Unique<Ast> factor = parse_factor();

    if (factor->type == Ast::None)
    {
        return factor;
    }

    bool stop = false;

    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::Dot:
        {
            Unique<Ast> access = std::make_unique<Ast>(Ast::Access);
            access->children.push_back(std::move(factor));

            Token token = buffer.next_token();

            if (token.type != Token::Identifier)
            {
                Error::error_in_line("Expected identifier", token, file);
            }

            access->token = token;
            factor = std::move(access);
            buffer.pop_state();
            break;
        }

        case Token::LParen:
        {
            Unique<Ast> call = std::make_unique<Ast>(Ast::Call);
            call->children.push_back(std::move(factor));
            call->children.push_back(parse_expr_list());
            factor = std::move(call);
            buffer.push_ignore_newlines_state(true);
            token = buffer.next_token();
            if (token.type != Token::RParen)
            {
                Error::error_in_line("Expected ')'", token, file);
            }
            buffer.pop_ignore_newlines_state();
            buffer.pop_state();
            break;
        }

        case Token::LBracket:
        {
            Unique<Ast> index = std::make_unique<Ast>(Ast::Subscript);
            index->children.push_back(std::move(factor));

            Unique<Ast> expr_list = parse_expr_list();

            if (expr_list->children.size() == 0)
            {
                expr_list = parse_type_list();
            }

            index->children.push_back(std::move(expr_list));

            factor = std::move(index);
            buffer.push_ignore_newlines_state(true);
            token = buffer.next_token();
            if (token.type != Token::RBracket)
            {
                Error::error_in_line("Expected ']'", token, file);
            }
            buffer.pop_ignore_newlines_state();
            buffer.pop_state();
            break;
        }

        default:
        {
            stop = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!stop);

    return factor;
}

Unique<Ast> Parser::parse_factor()
{
    Unique<Ast> factor = parse_literal();

    if (factor->type != Ast::None)
    {
        return factor;
    }

    buffer.push_state();
    Token token = buffer.next_token();

    if (token.type == Token::LParen)
    {
        factor = parse_expression(false);
        token = buffer.next_token();
        if (token.type != Token::RParen)
        {
            Error::error_in_line("Expected ')'", token, file);
        }

        buffer.pop_state();
    }
    else
    {
        buffer.back_to_state();
    }
    return factor;
}

Unique<Ast> Parser::parse_literal()
{
    buffer.push_state();
    Token token = buffer.next_token();

    switch (token.type)
    {
    case Token::Integer:
    {
        buffer.pop_state();
        return std::make_unique<Ast>(Ast::Integer, token);
    }

    case Token::Float:
    {
        buffer.pop_state();
        return std::make_unique<Ast>(Ast::Float, token);
    }

    case Token::String:
    {
        buffer.pop_state();
        return std::make_unique<Ast>(Ast::String, token);
    }

    case Token::True:
    case Token::False:
    {
        buffer.pop_state();
        return std::make_unique<Ast>(Ast::Boolean, token);
    }

    case Token::Null:
    {
        buffer.pop_state();
        return std::make_unique<Ast>(Ast::Null, token);
    }

    case Token::Identifier:
    {
        buffer.pop_state();
        return std::make_unique<Ast>(Ast::Identifier, token);
    }

    case Token::LBracket:
    {
        buffer.pop_state();
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
        buffer.pop_state();
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

    buffer.back_to_state();
    return std::make_unique<Ast>(Ast::None);
}

Unique<Ast> Parser::parse_struct_brace_initilizer()
{
    buffer.push_state();

    Token token = buffer.next_token();

    if (token.type != Token::LBrace)
    {
        buffer.back_to_state();
        return std::make_unique<Ast>(Ast::None);
    }

    buffer.pop_state();

    Token end_tok = buffer.next_token();

    if (end_tok.type != Token::RBrace)
    {
        Error::error_in_line("Expected '}'", token, file);
    }

    return std::make_unique<Ast>(Ast::None);
}
