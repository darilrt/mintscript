#include "mint/lexer.hpp"

#define TOKEN(type, value) Token(Token::Type::type, value, location)
#define CASE(c, r) \
    case c:        \
    {              \
        Next();    \
        return r;  \
    }
#define OPTION(c, r, d) peek() == c ? (next(), r) : d

bool is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\f';
}

bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

bool is_hex_digit(char c)
{
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_alpha_numeric(char c)
{
    return is_alpha(c) || is_digit(c);
}

Lexer::Lexer(std::streambuf *streambuf, const std::string &file)
    : streambuf(streambuf), location({.line = 1, .column = 1, .file = file})
{
}

Token Lexer::next_token()
{
    while (is_space(peek()))
    {
        next();
    }

    if (peek() == '\0')
        return Token{
            .type = Token::EndFile,
            .location = location,
        };

    if (is_alpha(peek()) || peek() == '_')
    {
        std::string v;

        while (is_alpha_numeric(peek()) || peek() == '_')
        {
            v += next();
        }

        return Token{
            .type = Token::Identifier,
            .value = v,
            .location = location,
        };
    }

    return Token{
        .type = Token::EndFile,
        .location = location,
    };
}

char Lexer::peek()
{
    return streambuf->sgetc();
}

char Lexer::next()
{
    char c = streambuf->sbumpc();
    location.column++;
    return c;
}

char Lexer::seek()
{
    char c = streambuf->snextc();
    return c;
}