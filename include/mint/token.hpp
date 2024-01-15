#pragma once

#include <string>

class Location
{
public:
    int line;
    int column;
    std::string file;

    std::string to_string() const;
};

class Token
{
public:
    enum Type
    {
        Unknown, // unknown token

        // literals
        Int,        // integer
        Float,      // float
        String,     // string
        Identifier, // identifier

        // keywords
        KeywordsBegin, // begin of keywords
        Null,          // null
        True,          // true
        False,         // false
        If,            // if
        Else,          // else
        Elif,          // elif
        While,         // while
        For,           // for
        In,            // in
        Break,         // break
        Continue,      // continue
        Return,        // ret
        Class,         // class
        Super,         // super
        As,            // as
        Interface,     // interface
        Enum,          // enum
        Import,        // import
        Mut,           // mut
        Let,           // let
        Impl,          // impl
        Ref,           // ref
        Export,        // export
        KeywordsEnd,   // end of keywords

        // operators
        Plus,                 // +
        Minus,                // -
        Star,                 // *
        Slash,                // /
        Percent,              // %
        Caret,                // ^
        Tilde,                // ~
        Not,                  // !
        Amp,                  // &
        Pipe,                 // |
        Less,                 // <
        Greater,              // >
        Dot,                  // .
        Comma,                // ,
        Colon,                // :
        Semicolon,            // ;
        Question,             // ?
        At,                   // @
        Dollar,               // $
        Hash,                 // #
        LParen,               // (
        RParen,               // )
        LBracket,             // [
        RBracket,             // ]
        LBrace,               // {
        RBrace,               // }
        Arrow,                // ->
        Ellipsis,             // ..
        Scope,                // ::
        Equal,                // =
        Backslash,            // \

        PlusPlus,             // ++
        MinusMinus,           // --
        StarStar,             // **
        AmpAmp,               // &&
        PipePipe,             // ||
        EqualEqual,           // ==
        LessLess,             // <<
        GreaterGreater,       // >>
        SlashSlash,           // //
        PlusAssign,           // +=
        MinusAssign,          // -=
        StarAssign,           // *=
        SlashAssign,          // /=
        PercentAssign,        // %=
        CaretAssign,          // ^=
        TildeAssign,          // ~=
        NotEqual,             // !=
        AmpAssign,            // &=
        PipeAssign,           // |=
        LessEqual,            // <=
        GreaterEqual,         // >=
        ColonAssign,          // :=
        LessLessAssign,       // <<=
        GreaterGreaterAssign, // >>=
        EqualEqualEqual,      // ===

        NewLine, // \n or \r\n or \r or \f or ;
        EndFile, // EOF
    };

    Type type = Unknown;
    std::string value;
    Location location;

    std::string to_string() const;
};