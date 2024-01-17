#include "mint/parser.hpp"
#include "mint/error.hpp"

std::unique_ptr<Ast> Parser::parse_program()
{
    std::vector<std::unique_ptr<Ast>> statements;

    buffer.push_ignore_newlines_state(false);

    auto stmt = parse_statement();

    if (stmt->type != Ast::None)
        statements.push_back(std::move(stmt));

    while (buffer.peek_token().type != Token::EndFile)
    {
        Token token = buffer.next_token();

        if (token.type != Token::NewLine)
            Error::error_in_line("Syntax error", token, file);

        stmt = parse_statement();

        if (stmt->type != Ast::None)
            statements.push_back(std::move(stmt));
    }

    buffer.pop_ignore_newlines_state();

    auto program = std::make_unique<Ast>(Ast::Program);
    program->children = std::move(statements);

    return program;
}

std::unique_ptr<Ast> Parser::parse_statement()
{
    return parse_expression(false);
}