#include "mint/lexer.hpp"

#define TOKEN(c, r)                                            \
    Token                                                      \
    {                                                          \
        Token::Type::c, r, Location                            \
        {                                                      \
            .line = location.line,                             \
            .column = location.column - std::string(r).size(), \
            .file = location.file                              \
        }                                                      \
    }

#define CASE(c, r) \
    case c:        \
    {              \
        next();    \
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
    {
        return Token{
            .type = Token::EndFile,
            .location = location,
        };
    }

    if (is_alpha(peek()) || peek() == '_')
    {
        const Token identifier = get_identifier_token();
        const Token keyword = get_keyword_from_identifier(identifier.value);

        if (keyword.type != Token::Unknown)
        {
            return keyword;
        }

        return identifier;
    }

    if (is_digit(peek()))
    {
        const Token number = get_number_token();

        if (peek() == 'f')
        {
            next();
            return Token{
                .type = Token::Float,
                .value = number.value,
                .location = number.location,
            };
        }

        return number;
    }

    if (peek() == '"')
    {
        next();
        return get_string_token('"');
    }

    if (peek() == '\'')
    {
        next();
        return get_string_token('\'');
    }

    if (peek() == '/' && seek() == '/')
    {
        next();
        next();

        while (peek() != '\n')
        {
            next();
        }

        return next_token();
    }

    switch (peek())
    {
        CASE('+',
             OPTION('=',
                    TOKEN(PlusAssign, "+="),
                    OPTION('+',
                           TOKEN(PlusPlus, "++"),
                           TOKEN(Plus, "+"))))

        CASE('-',
             OPTION('=',
                    TOKEN(MinusAssign, "-="),
                    OPTION('-',
                           TOKEN(MinusMinus, "--"),
                           OPTION('>',
                                  TOKEN(Arrow, "->"),
                                  TOKEN(Minus, "-")))))
        CASE('*',
             OPTION('=',
                    TOKEN(StarAssign, "*="),
                    OPTION('*',
                           TOKEN(StarStar, "**"),
                           TOKEN(Star, "*"))))
        CASE('/',
             OPTION('=',
                    TOKEN(SlashAssign, "/="),
                    OPTION('/',
                           TOKEN(SlashSlash, "//"),
                           TOKEN(Slash, "/"))))
        CASE('%',
             OPTION('=',
                    TOKEN(PercentAssign, "%="),
                    TOKEN(Percent, "%")))
        CASE('^',
             OPTION('=',
                    TOKEN(CaretAssign, "^="),
                    TOKEN(Caret, "^")))
        CASE('!',
             OPTION('=',
                    TOKEN(NotEqual, "!="),
                    TOKEN(Not, "!")))
        CASE('&',
             OPTION('=',
                    TOKEN(AmpAssign, "&="),
                    OPTION('&',
                           TOKEN(AmpAmp, "&&"),
                           TOKEN(Amp, "&"))))
        CASE('|',
             OPTION('=',
                    TOKEN(PipeAssign, "|="),
                    OPTION('|',
                           TOKEN(PipePipe, "||"),
                           TOKEN(Pipe, "|"))))
        CASE('<',
             OPTION('=',
                    TOKEN(LessEqual, "<="),
                    OPTION('<',
                           OPTION('=',
                                  TOKEN(LessLessAssign, "<<="),
                                  TOKEN(LessLess, "<<")),
                           TOKEN(Less, "<"))))
        CASE('>',
             OPTION('=',
                    TOKEN(GreaterEqual, ">="),
                    OPTION('>',
                           OPTION('=',
                                  TOKEN(GreaterGreaterAssign, ">>="),
                                  TOKEN(GreaterGreater, ">>")),
                           TOKEN(Greater, ">"))))
        CASE('=',
             OPTION('=',
                    OPTION('=',
                           TOKEN(EqualEqualEqual, "==="),
                           TOKEN(EqualEqual, "==")),
                    TOKEN(Equal, "=")))
        CASE('.',
             OPTION('.',
                    TOKEN(Ellipsis, ".."),
                    TOKEN(Dot, ".")))
        CASE(':',
             OPTION('=',
                    TOKEN(ColonAssign, ":="),
                    OPTION(':',
                           TOKEN(Scope, "::"),
                           TOKEN(Colon, ":"))))
        CASE('~',
             OPTION('=',
                    TOKEN(TildeAssign, "~="),
                    TOKEN(Tilde, "~")))
        CASE(',', TOKEN(Comma, ","))
        CASE(';', TOKEN(Semicolon, ";"))
        CASE('?', TOKEN(Question, "?"))
        CASE('@', TOKEN(At, "@"))
        CASE('$', TOKEN(Dollar, "$"))
        CASE('#', TOKEN(Hash, "#"))
        CASE('(', TOKEN(LParen, "("))
        CASE(')', TOKEN(RParen, ")"))
        CASE('[', TOKEN(LBracket, "["))
        CASE(']', TOKEN(RBracket, "]"))
        CASE('{', TOKEN(LBrace, "{"))
        CASE('}', TOKEN(RBrace, "}"))
        CASE('\\', TOKEN(Backslash, "\\"))
    case '\n':
        auto t = Token{
            .type = Token::NewLine,
            .value = " ",
            .location = {
                .line = location.line - 1,
                .column = location.column++,
                .file = location.file,
            },
        };
        next();
        return t;
    };

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
    if (c == '\n')
    {
        location.line++;
        location.column = 1;
    }
    return c;
}

char Lexer::seek()
{
    char c = streambuf->snextc();
    return c;
}

Token Lexer::get_number_token()
{
    std::string v;

    bool is_float = false;
    bool is_hex = false;
    bool is_binary = false;
    bool is_octal = false;

    if (peek() == '0')
    {
        v += next();

        switch (peek())
        {
        case 'x':
        case 'X':
            is_hex = true;
            v += next();
            break;
        case 'b':
        case 'B':
            is_binary = true;
            v += next();
            break;
        case 'o':
        case 'O':
            is_octal = true;
            v += next();
            break;
        }

        if (is_hex)
        {
            while (is_hex_digit(peek()))
            {
                v += next();
            }
        }
        else if (is_binary)
        {
            while (peek() == '0' || peek() == '1')
            {
                v += next();
            }
        }
        else if (is_octal)
        {
            while (peek() >= '0' && peek() <= '7')
            {
                v += next();
            }
        }
        else
        {
            while (is_digit(peek()))
            {
                v += next();
            }

            if (peek() == '.')
            {
                is_float = true;
                v += next();

                while (is_digit(peek()))
                {
                    v += next();
                }
            }
        }
    }
    else
    {
        while (is_digit(peek()))
        {
            v += next();
        }

        if (peek() == '.')
        {
            is_float = true;
            v += next();

            while (is_digit(peek()))
            {
                v += next();
            }
        }
    }

    return Token{
        .type = is_float ? Token::Float : Token::Integer,
        .value = v,
        .location = {
            .line = location.line,
            .column = location.column - v.size(),
            .file = location.file,
        },
    };
}

Token Lexer::get_string_token(char delimiter)
{
    std::string v;

    while (peek() != delimiter)
    {
        if (peek() == '\0')
        {
            return Token{
                .type = Token::EndFile,
                .location = location,
            };
        }

        if (peek() == '\\')
        {
            next();

            switch (peek())
            {
            case 'n':
                v += '\n';
                break;
            case 'r':
                v += '\r';
                break;
            case 't':
                v += '\t';
                break;
            case 'f':
                v += '\f';
                break;
            case 'b':
                v += '\b';
                break;
            case 'a':
                v += '\a';
                break;
            case 'v':
                v += '\v';
                break;
            case '\\':
                v += '\\';
                break;
            case '\'':
                v += '\'';
                break;
            case '"':
                v += '"';
                break;
            case '?':
                v += '?';
                break;
            case 'x':
            {
                next();

                if (!is_hex_digit(peek()))
                {
                    return Token{
                        .type = Token::EndFile,
                        .location = location,
                    };
                }

                char c = 0;

                while (is_hex_digit(peek()))
                {
                    c *= 16;

                    if (peek() >= '0' && peek() <= '9')
                    {
                        c += peek() - '0';
                    }
                    else if (peek() >= 'a' && peek() <= 'f')
                    {
                        c += peek() - 'a' + 10;
                    }
                    else if (peek() >= 'A' && peek() <= 'F')
                    {
                        c += peek() - 'A' + 10;
                    }

                    next();
                }

                v += c;
                break;
            }
            case '0':
            {
                next();

                if (is_digit(peek()))
                {
                    return Token{
                        .type = Token::EndFile,
                        .location = location,
                    };
                }

                v += '\0';
                break;
            }
            default:
                return Token{
                    .type = Token::EndFile,
                    .location = location,
                };
            }
        }
        else
        {
            v += next();
        }
    }

    next();

    return Token{
        .type = Token::String,
        .value = "\"" + v + "\"",
        .location = location,
    };
}

Token Lexer::get_identifier_token()
{
    std::string v;

    while (is_alpha_numeric(peek()) || peek() == '_')
    {
        v += next();
    }

    return Token{
        .type = Token::Identifier,
        .value = v,
        .location = {
            .line = location.line,
            .column = location.column - v.size(),
            .file = location.file,
        },
    };
}

Token Lexer::get_keyword_from_identifier(const std::string &value)
{
    if (value == "null")
    {
        return TOKEN(Null, value);
    }
    else if (value == "true")
    {
        return TOKEN(True, value);
    }
    else if (value == "false")
    {
        return TOKEN(False, value);
    }
    else if (value == "if")
    {
        return TOKEN(If, value);
    }
    else if (value == "else")
    {
        return TOKEN(Else, value);
    }
    else if (value == "elif")
    {
        return TOKEN(Elif, value);
    }
    else if (value == "while")
    {
        return TOKEN(While, value);
    }
    else if (value == "for")
    {
        return TOKEN(For, value);
    }
    else if (value == "in")
    {
        return TOKEN(In, value);
    }
    else if (value == "break")
    {
        return TOKEN(Break, value);
    }
    else if (value == "continue")
    {
        return TOKEN(Continue, value);
    }
    else if (value == "return")
    {
        return TOKEN(Return, value);
    }
    else if (value == "class")
    {
        return TOKEN(Class, value);
    }
    else if (value == "as")
    {
        return TOKEN(As, value);
    }
    else if (value == "interface")
    {
        return TOKEN(Interface, value);
    }
    else if (value == "enum")
    {
        return TOKEN(Enum, value);
    }
    else if (value == "import")
    {
        return TOKEN(Import, value);
    }
    else if (value == "mut")
    {
        return TOKEN(Mut, value);
    }
    else if (value == "let")
    {
        return TOKEN(Let, value);
    }
    else if (value == "new")
    {
        return TOKEN(New, value);
    }
    else if (value == "export")
    {
        return TOKEN(Export, value);
    }

    return TOKEN(Unknown, value);
}