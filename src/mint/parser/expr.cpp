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
    Unique<Ast> expression = parse_logical_and();
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

Unique<Ast> Parser::parse_comparison()
{
    Unique<Ast> lhs = parse_shift();

    if (lhs->type == Ast::None)
    {
        return lhs;
    }

    buffer.push_state();
    Token token = buffer.next_token();

    switch (token.type)
    {
    case Token::EqualEqual:
    case Token::NotEqual:
    case Token::Less:
    case Token::LessEqual:
    case Token::Greater:
    case Token::GreaterEqual:
    {
        Unique<Ast> comparison = std::make_unique<Ast>(Ast::BinaryOperator);
        comparison->token = token;
        comparison->children.push_back(std::move(lhs));
        comparison->children.push_back(parse_shift());
        lhs = std::move(comparison);
        buffer.pop_state();
        break;
    }

    default:
    {
        buffer.back_to_state();
        break;
    }
    }

    return lhs;
}

Unique<Ast> Parser::parse_unary()
{
    buffer.push_state();
    Token token = buffer.next_token();

    switch (token.type)
    {
    case Token::Plus:
    case Token::Minus:
    case Token::Not:
    case Token::Tilde:
    case Token::PlusPlus:
    case Token::MinusMinus:
    {
        Unique<Ast> unary = std::make_unique<Ast>(Ast::Prefix);
        unary->token = token;

        Unique<Ast> postfix = parse_postfix();

        if (postfix->type == Ast::None)
        {
            Error::error_in_line("Expected expression after unary operator", token, file);
        }

        unary->children.push_back(std::move(postfix));
        buffer.pop_state();
        return unary;
    }

    default:
    {
        buffer.back_to_state();
        return parse_postfix();
    }
    }
}

Unique<Ast> Parser::parse_postfix()
{
    Unique<Ast> postfix = parse_instancing();

    if (postfix->type == Ast::None)
    {
        return postfix;
    }

    buffer.push_state();
    Token token = buffer.next_token();

    switch (token.type)
    {
    case Token::PlusPlus:
    case Token::MinusMinus:
    {
        Unique<Ast> unary = std::make_unique<Ast>(Ast::Postfix);
        unary->token = token;
        unary->children.push_back(std::move(postfix));
        postfix = std::move(unary);
        buffer.pop_state();
        break;
    }

    case Token::As:
    {
        Unique<Ast> cast = std::make_unique<Ast>(Ast::Cast);
        cast->token = token;
        cast->children.push_back(std::move(postfix));
        Unique<Ast> type = parse_type();

        if (type->type == Ast::None)
        {
            Error::error_in_line("Expected type", token, file);
        }

        cast->children.push_back(std::move(type));
        postfix = std::move(cast);
        buffer.pop_state();
        break;
    }

    default:
    {
        buffer.back_to_state();
        break;
    }
    }

    return postfix;
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

            buffer.push_state();
            buffer.push_ignore_newlines_state(true);
            Unique<Ast> expr_list = parse_expression(true);

            Token close = buffer.next_token();
            if (close.type == Token::RBracket)
            {
                buffer.pop_ignore_newlines_state();
                index->children.push_back(std::move(expr_list));
                factor = std::move(index);
                buffer.pop_state();
                break;
            }
            buffer.back_to_state();

            expr_list = parse_type_list();

            index->children.push_back(std::move(expr_list));

            factor = std::move(index);
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
        buffer.push_ignore_newlines_state(true);
        Unique<Ast> array = std::make_unique<Ast>(Ast::Array);
        array->children.push_back(parse_expr_list());
        token = buffer.next_token();
        if (token.type != Token::RBracket)
        {
            Error::error_in_line("Expected ']'", token, file);
        }
        buffer.pop_ignore_newlines_state();
        return array;
    }

    case Token::LBrace:
    {
        buffer.pop_state();
        buffer.push_ignore_newlines_state(true);
        Unique<Ast> dict = parse_dict();
        token = buffer.next_token();
        if (token.type != Token::RBrace)
        {
            Error::error_in_line("Expected '}'", token, file);
        }
        buffer.pop_ignore_newlines_state();
        return dict;
    }

    default:
        break;
    }

    buffer.back_to_state();
    return std::make_unique<Ast>(Ast::None);
}

Unique<Ast> Parser::parse_dict()
{
    Unique<Ast> dict_list = std::make_unique<Ast>(Ast::Dict);

    buffer.push_ignore_newlines_state(true);
    do
    {
        Unique<Ast> dict_pair = parse_dict_pair();

        if (dict_pair->type == Ast::None)
        {
            break;
        }

        dict_list->children.push_back(std::move(dict_pair));

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

    return dict_list;
}

Unique<Ast> Parser::parse_dict_pair()
{
    Unique<Ast> key = parse_expression(true);

    if (key->type == Ast::None)
    {
        return key;
    }

    buffer.push_state();
    Token token = buffer.next_token();

    if (token.type != Token::Colon)
    {
        Error::error_in_line("Expected ':' after key", token, file);
    }
    buffer.pop_state();

    Unique<Ast> value = parse_expression(true);

    if (value->type == Ast::None)
    {
        Error::error_in_line("Expected value after ':'", token, file);
    }

    Unique<Ast> pair = std::make_unique<Ast>(Ast::DictPair);
    pair->children.push_back(std::move(key));
    pair->children.push_back(std::move(value));

    return pair;
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

    Unique<Ast> struct_brace_initializer = std::make_unique<Ast>(Ast::StructBraceInitializer);

    buffer.push_ignore_newlines_state(true);

    do
    {
        Unique<Ast> pair = parse_struct_brace_initilizer_pair();

        if (pair->type == Ast::None)
        {
            break;
        }

        struct_brace_initializer->children.push_back(std::move(pair));

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

    buffer.pop_ignore_newlines_state();

    Token end_tok = buffer.next_token();

    if (end_tok.type != Token::RBrace)
    {
        Error::error_in_line("Expected '}'", token, file);
    }

    return struct_brace_initializer;
}

Unique<Ast> Parser::parse_struct_brace_initilizer_pair()
{
    buffer.push_state();
    Token token = buffer.next_token();

    if (token.type != Token::Dot)
    {
        buffer.back_to_state();
        return std::make_unique<Ast>(Ast::None);
    }

    buffer.pop_state();

    Unique<Ast> pair = std::make_unique<Ast>(Ast::StructBraceInitializerPair);

    Token ident = buffer.next_token();

    if (ident.type != Token::Identifier)
    {
        Error::error_in_line("Expected identifier", token, file);
    }

    pair->token = ident;

    token = buffer.next_token();

    if (token.type != Token::Equal)
    {
        Error::error_in_line("Expected '=' after identifier", ident, file);
    }

    pair->children.push_back(parse_expression(true));

    if (pair->children[0]->type == Ast::None)
    {
        Error::error_in_line("Expected expression after '='", token, file);
    }

    return pair;
}