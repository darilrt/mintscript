#include "lexer.h"

#include <cctype>
#include <iostream>

#define TOKEN(type, value) Token(Token::Type::type, value, location)
#define CASE(c, r) case c: { Next(); return r; }
#define OPTION(c, r, d) Peek() == c ? (Next(), r) : d

bool IsSpace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\f';
}

bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

bool IsHexDigit(char c) {
    return IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool IsAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool IsAlphaNumeric(char c) {
    return IsAlpha(c) || IsDigit(c);
}

Lexer::Lexer(const std::string& source, const std::string& filename) : source(source) {
    location.line = 1;
    location.column = 0;
    location.file = "<stdin>";

    if (filename != "") {
        location.file = filename;
    }
}

char Lexer::Next() {
    char c = source[pos++];
    if (c == '\n') {
        location.line++;
        location.column = 0;
    } else {
        location.column++;
    }

    return c;
}

Token Lexer::NextToken() {
    std::string value = "";

    while (IsSpace(Peek())) {
        Next();
    }

    if (Peek() == '\0') {
        return TOKEN(EndFile, "EOF");
    }

    if (IsAlpha(Peek()) || Peek() == '_') {
        while (IsAlphaNumeric(Peek()) || Peek() == '_') {
            value += Next();
        }

        const Token token = GetKeywordToken(value);

        if (token.type != Token::Type::Unknown) {
            return token;
        }

        return TOKEN(Identifier, value);
    }

    if (IsDigit(Peek())) {
        return GetNumberToken();
    }

    if (Peek() == '"') {
        return GetStringToken(true);
    }

    if (Peek() == '\'') {
        return GetStringToken(false);
    }

    if (Peek() == '/') {
        Next();

        if (Peek() == '/') {
            while (Peek() != '\n') {
                Next();
            }
            Next();

            return NextToken();
        }
        else if (Peek() == '*') {
            Next();
            Next();

            while (true) {
                if (Peek() == '*') {
                    Next();

                    if (Peek() == '/') {
                        Next();
                        break;
                    }
                } else {
                    Next();
                }
            }



            return NextToken();
        }
    }

    switch (Peek()) {
        CASE('+', 
            OPTION('=', 
                TOKEN(PlusAssign, "+="), 
                OPTION('+', 
                    TOKEN(PlusPlus, "++"), 
                    TOKEN(Plus, "+")
                )
            )
        )
        CASE('-', 
            OPTION('=', 
                TOKEN(MinusAssign, "-="), 
                OPTION('-', 
                    TOKEN(MinusMinus, "--"), 
                    OPTION('>', 
                        TOKEN(Arrow, "->"), 
                        TOKEN(Minus, "-")
                    )
                )
            )
        )
        CASE('*', 
            OPTION('=', 
                TOKEN(StarAssign, "*="), 
                OPTION('*', 
                    TOKEN(StarStar, "**"), 
                    TOKEN(Star, "*")
                )
            )
        )
        CASE('/', 
            OPTION('=', 
                TOKEN(SlashAssign, "/="), 
                OPTION('/', 
                    TOKEN(SlashSlash, "//"), 
                    TOKEN(Slash, "/")
                )
            )
        )
        CASE('%', 
            OPTION('=', 
                TOKEN(PercentAssign, "%="), 
                TOKEN(Percent, "%")
            )
        )
        CASE('^', 
            OPTION('=', 
                TOKEN(CaretAssign, "^="), 
                TOKEN(Caret, "^")
            )
        )
        CASE('!', 
            OPTION('=', 
                TOKEN(NotEqual, "!="), 
                TOKEN(Not, "!")
            )
        )
        CASE('&', 
            OPTION('=', 
                TOKEN(AmpAssign, "&="), 
                OPTION('&', 
                    TOKEN(AmpAmp, "&&"), 
                    TOKEN(Amp, "&")
                )
            )
        )
        CASE('|', 
            OPTION('=', 
                TOKEN(PipeAssign, "|="), 
                OPTION('|', 
                    TOKEN(PipePipe, "||"), 
                    TOKEN(Pipe, "|")
                )
            )
        )
        CASE('<', 
            OPTION('=', 
                TOKEN(LessEqual, "<="), 
                OPTION('<', 
                    OPTION('=', 
                        TOKEN(LessLessAssign, "<<="), 
                        TOKEN(LessLess, "<<")
                    ), 
                    TOKEN(Less, "<")
                )
            )
        )
        CASE('>', 
            OPTION('=', 
                TOKEN(GreaterEqual, ">="), 
                OPTION('>', 
                    OPTION('=', 
                        TOKEN(GreaterGreaterAssign, ">>="), 
                        TOKEN(GreaterGreater, ">>")
                    ), 
                    TOKEN(Greater, ">")
                )
            )
        )
        CASE('=', 
            OPTION('=', 
                OPTION('=', 
                    TOKEN(EqualEqualEqual, "==="), 
                    TOKEN(EqualEqual, "==")
                ),
                TOKEN(Equal, "=")
            )
        )
        CASE('.', 
            OPTION('.', 
                TOKEN(Ellipsis, ".."), 
                TOKEN(Dot, ".")
            )
        )
        CASE(':', 
            OPTION('=', 
                TOKEN(ColonAssign, ":="), 
                OPTION(':', 
                    TOKEN(Scope, "::"), 
                    TOKEN(Colon, ":")
                )
            )
        )
        CASE('~', 
            OPTION('=',
                TOKEN(TildeAssign, "~="),
                TOKEN(Tilde, "~")
            )
        )
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
        CASE('\n', TOKEN(NewLine, "LF"))
    }

    return TOKEN(EndFile, "EOF");
}

Token Lexer::GetKeywordToken(const std::string &value) {
    if (value == "null") { return TOKEN(Null, value); }
    else if (value == "true") { return TOKEN(True, value); }
    else if (value == "false") { return TOKEN(False, value); }
    else if (value == "if") { return TOKEN(If, value); }
    else if (value == "else") { return TOKEN(Else, value); }
    else if (value == "elif") { return TOKEN(Elif, value); }
    else if (value == "while") { return TOKEN(While, value); }
    else if (value == "for") { return TOKEN(For, value); }
    else if (value == "in") { return TOKEN(In, value); }
    else if (value == "break") { return TOKEN(Break, value); }
    else if (value == "continue") { return TOKEN(Continue, value); }
    else if (value == "return") { return TOKEN(Return, value); }
    else if (value == "class") { return TOKEN(Class, value); }
    else if (value == "super") { return TOKEN(Super, value); }
    else if (value == "as") { return TOKEN(As, value); }
    else if (value == "interface") { return TOKEN(Interface, value); }
    else if (value == "enum") { return TOKEN(Enum, value); }
    else if (value == "import") { return TOKEN(Import, value); }
    else if (value == "mut") { return TOKEN(Mut, value); }
    else if (value == "let") { return TOKEN(Let, value); }
    else if (value == "impl") { return TOKEN(Impl, value); }
    else if (value == "ref") { return TOKEN(Ref, value); }
    else if (value == "export") { return TOKEN(Export, value); }

    return TOKEN(Unknown, value);
}

Token Lexer::GetNumberToken() {
    std::string value = "";

    bool isHex = false;
    bool isOctal = false;
    bool isBinary = false;
    bool isFloat = false;
    bool isScientific = false;
    bool isSuffix = false;

    // Check for a prefix
    if (Peek() == '0') {
        value += Next();

        if (Peek() == 'x') {
            value += Next();
            isHex = true;
        } else if (Peek() == 'b') {
            value += Next();
            isBinary = true;
        } else if (Peek() == 'o') {
            value += Next();
            isOctal = true;
        }
    }

    // Get the number value if is a hex, octal, or binary number
    if (isHex) {
        while (IsHexDigit(Peek())) {
            value += Next();
        }
        return TOKEN(Int, value);
    } else if (isOctal) {
        while (Peek() >= '0' && Peek() <= '7') {
            value += Next();
        }
        return TOKEN(Int, value);
    } else if (isBinary) {
        while (Peek() == '0' || Peek() == '1') {
            value += Next();
        }
        return TOKEN(Int, value);
    } else {
        while (IsDigit(Peek())) {
            value += Next();
        }

        // Check for a decimal point
        if (Peek() == '.') {
            isFloat = true;
            value += Next();

            while (IsDigit(Peek())) {
                value += Next();
            }
        }

        // Check for scientific notation
        if (Peek() == 'e' || Peek() == 'E') {
            isScientific = true;
            value += Next();

            if (Peek() == '+' || Peek() == '-') {
                value += Next();
            }

            while (IsDigit(Peek())) {
                value += Next();
            }
        } else if (Peek() == 'f') {
            isSuffix = true;
            value += Next();
        }
    }

    if (isSuffix || isFloat || isScientific) {
        return TOKEN(Float, value);
    }

    return TOKEN(Int, value);
}

Token Lexer::GetStringToken(bool doubleQuote) {
    const char quote = doubleQuote ? '"' : '\'';

    std::string value = "";

    // Skip the opening quote
    Next();

    while (Peek() != quote && Peek() != EOF) {
        if (Peek() == '\\') {
            Next();
            
            switch (Peek()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '0': value += '\0'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                case '\\': value += '\\'; break;
                default: value += Peek(); break;
            }

            // TODO: Handle unicode characters
            // TODO: Error handling for invalid escape sequences

            Next();

        } else {
            value += Next();
        }
    }

    // Skip the closing quote
    Next();

    return TOKEN(String, value);
}
