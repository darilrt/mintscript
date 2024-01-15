#include "mint/token_buffer.hpp"

TokenBuffer::TokenBuffer(std::streambuf *streambuf, const std::string &file)
    : lexer(streambuf, file), ignore_newlines(false), index(0)
{
    Token token = lexer.next_token();

    while (token.type != Token::EndFile)
    {
        tokens.push_back(token);
        token = lexer.next_token();
    }

    tokens.push_back(token);
}

Token TokenBuffer::next_token()
{
    Token token = peek_token();

    if (token.type != Token::EndFile)
    {
        index++;
    }

    if (ignore_newlines)
    {
        while (token.type == Token::NewLine)
        {
            token = peek_token();
            if (token.type != Token::EndFile)
            {
                index++;
            }
        }
    }

    return token;
}

Token TokenBuffer::peek_token()
{
    if (index < tokens.size())
    {
        return tokens[index];
    }
    return Token{
        .type = Token::EndFile,
    };
}

void TokenBuffer::push_state()
{
    states.push_back(index);
}

void TokenBuffer::pop_state()
{
    index = states.back();
    states.pop_back();
}

void TokenBuffer::push_ignore_newlines_state(bool ignore_newlines)
{
    ignore_newlines_states.push_back(this->ignore_newlines);
    this->ignore_newlines = ignore_newlines;
}

void TokenBuffer::pop_ignore_newlines_state()
{
    this->ignore_newlines = ignore_newlines_states.back();
    ignore_newlines_states.pop_back();
}