#include <memory>
#include <vector>
#include <string>

#include "mint/error.hpp"
#include "mint/parser.hpp"
#include "mint/token.hpp"
#include "mint/token_buffer.hpp"

// <expr_list> ::= <expression> { "," { <expression> } }
std::unique_ptr<Ast> Parser::parse_expr_list()
{
    std::unique_ptr<Ast> ast = std::make_unique<Ast>(Ast::ExprList);

    std::unique_ptr<Ast> expr = parse_expression(true);

    if (expr->type == Ast::None)
    {
        Error::error_in_line("Expected expression", buffer.next_token(), file);
    }

    ast->children.push_back(std::move(expr));

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::Comma:
        {
            expr = parse_expression(true);

            if (expr->type == Ast::None)
            {
                done = true;
                buffer.pop_state();
                break;
            }

            ast->children.push_back(std::move(expr));
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }

    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_expression(bool ignore_newlines)
{
    buffer.push_ignore_newlines_state(ignore_newlines);
    auto ast = parse_logical_or();
    buffer.pop_ignore_newlines_state();

    return ast;
}

std::unique_ptr<Ast> Parser::parse_logical_or()
{
    std::unique_ptr<Ast> ast = parse_logical_and();

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::PipePipe:
        {
            auto binary = std::make_unique<Ast>(Ast::BinaryOperator);
            binary->token = token;
            binary->children.push_back(std::move(ast));
            binary->children.push_back(parse_logical_and());

            if (binary->children[1]->type == Ast::None)
            {
                Error::error_in_line("Expected expression after binary operator", token, file);
            }

            ast = std::move(binary);
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_logical_and()
{
    std::unique_ptr<Ast> ast = parse_bitwise_or();

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::AmpAmp:
        {
            auto binary = std::make_unique<Ast>(Ast::BinaryOperator);
            binary->token = token;
            binary->children.push_back(std::move(ast));
            binary->children.push_back(parse_bitwise_or());

            if (binary->children[1]->type == Ast::None)
            {
                Error::error_in_line("Expected expression after binary operator", token, file);
            }

            ast = std::move(binary);
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_bitwise_or()
{
    std::unique_ptr<Ast> ast = parse_bitwise_xor();

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::Pipe:
        {
            auto binary = std::make_unique<Ast>(Ast::BinaryOperator);
            binary->token = token;
            binary->children.push_back(std::move(ast));
            binary->children.push_back(parse_bitwise_xor());

            if (binary->children[1]->type == Ast::None)
            {
                Error::error_in_line("Expected expression after binary operator", token, file);
            }

            ast = std::move(binary);
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_bitwise_xor()
{
    std::unique_ptr<Ast> ast = parse_bitwise_and();

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::Caret:
        {
            auto binary = std::make_unique<Ast>(Ast::BinaryOperator);
            binary->token = token;
            binary->children.push_back(std::move(ast));
            binary->children.push_back(parse_bitwise_and());

            if (binary->children[1]->type == Ast::None)
            {
                Error::error_in_line("Expected expression after binary operator", token, file);
            }

            ast = std::move(binary);
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_bitwise_and()
{
    std::unique_ptr<Ast> ast = parse_comparison();

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::Amp:
        {
            auto binary = std::make_unique<Ast>(Ast::BinaryOperator);
            binary->token = token;
            binary->children.push_back(std::move(ast));
            binary->children.push_back(parse_comparison());

            if (binary->children[1]->type == Ast::None)
            {
                Error::error_in_line("Expected expression after binary operator", token, file);
            }

            ast = std::move(binary);
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_comparison()
{
    std::unique_ptr<Ast> ast = parse_shift();

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::EqualEqual:
        case Token::NotEqual:
        case Token::Less:
        case Token::Greater:
        case Token::LessEqual:
        case Token::GreaterEqual:
        {
            auto binary = std::make_unique<Ast>(Ast::BinaryOperator);
            binary->token = token;
            binary->children.push_back(std::move(ast));
            binary->children.push_back(parse_shift());

            if (binary->children[1]->type == Ast::None)
            {
                Error::error_in_line("Expected expression after binary operator", token, file);
            }

            ast = std::move(binary);
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_shift()
{
    std::unique_ptr<Ast> ast = parse_additive();

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::LessLess:
        case Token::GreaterGreater:
        {
            auto binary = std::make_unique<Ast>(Ast::BinaryOperator);
            binary->token = token;
            binary->children.push_back(std::move(ast));
            binary->children.push_back(parse_additive());

            if (binary->children[1]->type == Ast::None)
            {
                Error::error_in_line("Expected expression after binary operator", token, file);
            }

            ast = std::move(binary);
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_additive()
{
    std::unique_ptr<Ast> ast = parse_multiplicative();

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::Plus:
        case Token::Minus:
        {
            auto binary = std::make_unique<Ast>(Ast::BinaryOperator);
            binary->token = token;
            binary->children.push_back(std::move(ast));
            binary->children.push_back(parse_multiplicative());

            if (binary->children[1]->type == Ast::None)
            {
                Error::error_in_line("Expected expression after binary operator", token, file);
            }

            ast = std::move(binary);
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_multiplicative()
{
    std::unique_ptr<Ast> ast = parse_unary();

    bool done = false;
    do
    {
        buffer.push_state();
        Token token = buffer.next_token();

        switch (token.type)
        {
        case Token::Star:
        case Token::Slash:
        case Token::Percent:
        {
            auto binary = std::make_unique<Ast>(Ast::BinaryOperator);
            binary->token = token;
            binary->children.push_back(std::move(ast));
            binary->children.push_back(parse_unary());

            if (binary->children[1]->type == Ast::None)
            {
                Error::error_in_line("Expected expression after binary operator", token, file);
            }

            ast = std::move(binary);
            buffer.pop_state();
            break;
        }

        default:
        {
            done = true;
            buffer.back_to_state();
            break;
        }
        }
    } while (!done);

    return ast;
}

std::unique_ptr<Ast> Parser::parse_unary()
{
    std::unique_ptr<Ast> ast;

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
        ast = std::make_unique<Ast>(Ast::Prefix);
        ast->token = token;
        buffer.pop_state();
        break;
    }

    default:
    {
        buffer.back_to_state();
        break;
    }
    }

    if (ast)
        ast->children.push_back(parse_unary());
    else
        ast = parse_postfix();

    return ast;
}

std::unique_ptr<Ast> Parser::parse_postfix()
{
    std::unique_ptr<Ast> ast = parse_factor();

    buffer.push_state();
    Token token = buffer.next_token();

    switch (token.type)
    {
    case Token::PlusPlus:
    case Token::MinusMinus:
    case Token::As:
    {
        std::unique_ptr<Ast> postfix = std::make_unique<Ast>(Ast::Postfix);
        postfix->token = token;
        postfix->children.push_back(std::move(ast));
        ast = std::move(postfix);

        if (token.type == Token::As)
        {
            std::unique_ptr<Ast> type = parse_type();

            if (type->type == Ast::None)
            {
                Error::error_in_line("Expected type after 'as'", token, file);
            }

            ast->children.push_back(std::move(type));
        }
        buffer.pop_state();
        break;
    }

    default:
    {
        buffer.back_to_state();
        break;
    }
    }

    return ast;
}

std::unique_ptr<Ast> Parser::parse_factor()
{
    std::unique_ptr<Ast> ast = std::make_unique<Ast>();

    buffer.push_state();
    Token token = buffer.next_token();

    switch (token.type)
    {
    case Token::Integer:
    {
        ast->type = Ast::Integer;
        ast->value = new int(std::stoi(token.value));
        buffer.pop_state();
        break;
    }
    case Token::Float:
    {
        ast->type = Ast::Float;
        ast->value = new float(std::stof(token.value));
        buffer.pop_state();
        break;
    }
    case Token::String:
    {
        ast->type = Ast::String;
        ast->value = new std::string(token.value);
        buffer.pop_state();
        break;
    }
    case Token::True:
    {
        ast->type = Ast::Boolean;
        ast->value = new bool(true);
        buffer.pop_state();
        break;
    }
    case Token::False:
    {
        ast->type = Ast::Boolean;
        ast->value = new bool(false);
        buffer.pop_state();
        break;
    }
    case Token::Identifier:
    {
        ast->type = Ast::Identifier;
        ast->token = token;
        buffer.pop_state();
        break;
    }
    case Token::LParen:
    {
        ast = parse_expression(true);

        Token cparen = buffer.next_token();

        switch (cparen.type)
        {
        case Token::RParen:
        {
            break;
        }
        case Token::EndFile:
        {
            Error::error_in_line("Unmatched '('", token, file);
            break;
        }
        default:
        {
            Error::error_in_line("Expected ')'", cparen, file);
            break;
        }
        }

        buffer.pop_state();
        break;
    }
    case Token::LBracket:
    {
        ast->type = Ast::Array;

        buffer.push_ignore_newlines_state(true);
        buffer.push_state();
        Token cbracket = buffer.next_token();

        switch (cbracket.type)
        {
        case Token::RBracket:
        {
            ast->children.push_back(std::make_unique<Ast>(Ast::ExprList));
            buffer.pop_state();
            buffer.pop_state();
            buffer.pop_ignore_newlines_state();
            return ast;
        }
        case Token::EndFile:
        {
            Error::error_in_line("Unmatched '['", token, file);
        }
        }
        buffer.back_to_state();

        ast->children.push_back(parse_expr_list());

        cbracket = buffer.next_token();
        switch (cbracket.type)
        {
        case Token::RBracket:
        {
            break;
        }
        case Token::EndFile:
        {
            Error::error_in_line("Unmatched '['", token, file);
        }
        default:
        {
            Error::error_in_line("Expected ']'", cbracket, file);
        }
        }

        buffer.pop_state();
        buffer.pop_ignore_newlines_state();
        break;
    }
    case Token::New:
    {
        ast->type = Ast::Instantiation;
        ast->token = token;

        ast->children.push_back(parse_type());

        if (ast->children[0]->type == Ast::None)
        {
            Error::error_in_line("Expected type after 'new'", token, file);
        }

        buffer.pop_state();
        break;
    }
    default:
    {
        buffer.back_to_state();
        break;
    }
    }

    return ast;
}