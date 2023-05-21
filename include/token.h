#pragma once

#include <string>

class Location {
public:
    Location() = default;
    Location(uint32_t line, uint32_t col) : line(line), column(col) {}
    Location(uint32_t line, uint32_t col, const std::string& file) : line(line), column(col), file(file) {}

    std::string ToString() const;

    uint32_t line = 1;
    uint32_t column = 1;
    std::string file = "";
};

class Token {
public:
    enum class Type {
        Unknown, // unknown token

        // literals
        Int, // integer
        Float, // float
        String, // string
        Identifier, // identifier

        // keywords
        KeywordsBegin, // begin of keywords
        Null, // null
        True, // true
        False, // false
        If, // if
        Else, // else
        Elif, // elif
        While, // while
        For, // for
        In, // in
        Break, // break
        Continue, // continue
        Return, // ret
        Class, // class
        Super, // super
        As, // as
        Interface, // interface
        Enum, // enum
        Import, // import
        Mut, // mut
        Let, // let
        Const, // const
        Ref, // ref
        Module, // module
        Export, // export
        KeywordsEnd, // end of keywords

        // operators
        Plus, // +
        Minus, // -
        Star, // *
        Slash, // /
        Percent, // %
        Caret, // ^
        Tilde, // ~
        Not, // !
        Amp, // &
        Pipe, // |
        Less, // <
        Greater, // >
        Dot, // .
        Comma, // ,
        Colon, // :
        Semicolon, // ;
        Question, // ?
        At, // @
        Dollar, // $
        Hash, // #
        LParen, // (
        RParen, // )
        LBracket, // [
        RBracket, // ]
        LBrace, // {
        RBrace, // }
        Arrow, // ->
        Ellipsis, // ..
        Scope, // ::
        Equal, // =
        Backslash, // \

        PlusPlus, // ++
        MinusMinus, // --
        StarStar, // **
        AmpAmp, // &&
        PipePipe, // ||
        EqualEqual, // ==
        LessLess, // <<
        GreaterGreater, // >>
        SlashSlash, // //
        PlusAssign, // +=
        MinusAssign, // -=
        StarAssign, // *=
        SlashAssign, // /=
        PercentAssign, // %=
        CaretAssign, // ^=
        TildeAssign, // ~=
        NotEqual, // !=
        AmpAssign, // &=
        PipeAssign, // |=
        LessEqual, // <=
        GreaterEqual, // >=
        ColonAssign, // :=
        LessLessAssign, // <<=
        GreaterGreaterAssign, // >>=
        EqualEqualEqual, // ===

        NewLine, // \n or \r\n or \r or \f or ;
        EndFile, // EOF
    };

    Token() = default;
    Token(Type type) : type(type) {}
    Token(Type type, const std::string& value, const Location& location) : type(type), value(value), location(location) {}
    Token(Type type, const std::string& value, uint32_t line, uint32_t column) : type(type), value(value), location(line, column) {}
    Token(Type type, const std::string& value, uint32_t line, uint32_t column, const std::string& file) : type(type), value(value), location(line, column, file) {}

    std::string ToString() const;
    std::string TypeToString() const;

    Type type = Type::Unknown;
    std::string value;
    Location location;
};