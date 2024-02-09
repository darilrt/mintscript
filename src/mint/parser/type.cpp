
#include "mint/parser.hpp"

Unique<Ast> Parser::parse_type_list()
{
    Unique<Ast> type_list = std::make_unique<Ast>(Ast::TypeList);

    buffer.push_ignore_newlines_state(true);
    do
    {
        Unique<Ast> type = parse_type();

        if (type->type == Ast::None)
        {
            break;
        }

        type_list->children.push_back(std::move(type));

        buffer.push_state();
        Token token = buffer.next_token();

        if (token.type != Token::Comma)
        {
            buffer.back_to_state();
            break;
        }

        buffer.pop_state();

    } while (true);
    buffer.pop_ignore_newlines_state();

    return type_list;
}

Unique<Ast> Parser::parse_type()
{
    return parse_list_type();
}

Unique<Ast> Parser::parse_generic_type()
{
    Unique<Ast> node = std::make_unique<Ast>(Ast::Identifier);

    buffer.push_state();
    Token token = buffer.next_token();

    if (token.type != Token::Identifier)
    {
        buffer.back_to_state();
        return std::make_unique<Ast>(Ast::None);
    }

    node->token = token;

    buffer.pop_state();

    buffer.push_state();
    Token open = buffer.next_token();

    if (open.type != Token::LBracket)
    {
        buffer.back_to_state();
        return node;
    }
    buffer.pop_state();

    Unique<Ast> generic_type = std::make_unique<Ast>(Ast::Generic, open);
    generic_type->children.push_back(std::move(node));

    Unique<Ast> type_list = parse_type_list();
    generic_type->children.push_back(std::move(type_list));

    Token close = buffer.next_token();

    if (close.type != Token::RBracket)
    {
        Error::error_in_line("Expected ']' after '['", open, file);
    }

    return generic_type;
}

Unique<Ast> Parser::parse_list_type()
{
    buffer.push_state();
    Token token = buffer.next_token();

    if (token.type != Token::LBracket)
    {
        buffer.back_to_state();
        return parse_optional_type();
    }

    Token close = buffer.next_token();

    if (close.type != Token::RBracket)
    {
        Error::error_in_line("Expected ']' after '['", token, file);
    }

    buffer.pop_state();

    Unique<Ast> list_type = std::make_unique<Ast>(Ast::List, token);

    list_type->children.push_back(parse_optional_type());

    return list_type;
}

Unique<Ast> Parser::parse_optional_type()
{
    Unique<Ast> node = parse_dotted_type();

    if (node->type == Ast::None)
    {
        return node;
    }

    buffer.push_state();
    Token token = buffer.next_token();

    if (token.type != Token::Question)
    {
        buffer.back_to_state();
        return node;
    }

    buffer.pop_state();

    Unique<Ast> optional_type = std::make_unique<Ast>(Ast::Optional, token);
    optional_type->children.push_back(std::move(node));

    return optional_type;
}