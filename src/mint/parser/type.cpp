
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
        buffer.pop_state();
    }
    else
    {
        buffer.back_to_state();
    }

    return ast;
}