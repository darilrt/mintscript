
#include "mint/parser.hpp"

std::unique_ptr<Ast> Parser::parse_type()
{
    auto ast = std::make_unique<Ast>();

    buffer.push_state();
    Token token = buffer.next_token();

    if (token.type == Token::Identifier)
    {
        ast->type = Ast::Identifier;
        ast->token = token;
    }
    else
    {
        buffer.pop_state();
    }

    return ast;
}