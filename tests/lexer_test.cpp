#include "test.h"
#include "lexer.h"

#include <cassert>
#include <iostream>

#define ASSERT_TOKEN(t) assert(lexer.NextToken().type == Token::Type::t);
#define ASSERT_TOKEN_VALUE(t, v) { auto token = lexer.NextToken(); assert(token.type == Token::Type::t); assert(token.value == v);}
#define LOG_TOKEN(t) std::cout << "\n" << lexer.NextToken().ToString() << std::endl;

void TestLexerNumberLiterals() {
    std::string source = 
            "0x1234 " // hex
            "0o1234 " // octal
            "0b1010 " // binary
            "1234 " // integer
            "1234.5678 " // float
            "1234e10 " // integer scientific
            "1234.5678e10 " // float scientific
            "1234.5678f " // float suffix
            "1234f "; // float suffix

    Lexer lexer(source);

    ASSERT_TOKEN(Int) ASSERT_TOKEN(Int) ASSERT_TOKEN(Int) ASSERT_TOKEN(Int)
    ASSERT_TOKEN(Float) ASSERT_TOKEN(Float) ASSERT_TOKEN(Float) ASSERT_TOKEN(Float) ASSERT_TOKEN(Float)
    ASSERT_TOKEN(EndFile)
}

void TestLexerKeywords() {
    std::string source = "null true false if else elif while for in break continue ret class impl interface enum import mut let const ref module export";

    Lexer lexer(source);

    ASSERT_TOKEN(Null)
    ASSERT_TOKEN(True)
    ASSERT_TOKEN(False)
    ASSERT_TOKEN(If)
    ASSERT_TOKEN(Else)
    ASSERT_TOKEN(Elif)
    ASSERT_TOKEN(While)
    ASSERT_TOKEN(For)
    ASSERT_TOKEN(In)
    ASSERT_TOKEN(Break)
    ASSERT_TOKEN(Continue)
    ASSERT_TOKEN(Return)
    ASSERT_TOKEN(Class)
    ASSERT_TOKEN(Impl)
    ASSERT_TOKEN(Interface)
    ASSERT_TOKEN(Enum)
    ASSERT_TOKEN(Import)
    ASSERT_TOKEN(Mut)
    ASSERT_TOKEN(Let)
    ASSERT_TOKEN(Const)
    ASSERT_TOKEN(Ref)
    ASSERT_TOKEN(Module)
    ASSERT_TOKEN(Export)
    ASSERT_TOKEN(EndFile)
}

void TestLexerSymbols() {
    const std::string 
    source = "+ - * / % ^ ~ ! & | < > . , : ; ? @ $ # ( ) [ ] { } -> .. :: = "
             "++ -- && || == << >> "
             "+= -= *= /= %= ^= ~= != &= |= <= >= := "
             "<<= >>= === \\ \n";

    Lexer lexer(source);

    ASSERT_TOKEN(Plus)       ASSERT_TOKEN(Minus)     ASSERT_TOKEN(Star)      ASSERT_TOKEN(Slash)
    ASSERT_TOKEN(Percent)    ASSERT_TOKEN(Caret)     ASSERT_TOKEN(Tilde)     ASSERT_TOKEN(Not)
    ASSERT_TOKEN(Amp)        ASSERT_TOKEN(Pipe)      ASSERT_TOKEN(Less)      ASSERT_TOKEN(Greater) 
    ASSERT_TOKEN(Dot)        ASSERT_TOKEN(Comma)     ASSERT_TOKEN(Colon)     ASSERT_TOKEN(Semicolon)
    ASSERT_TOKEN(Question)   ASSERT_TOKEN(At)        ASSERT_TOKEN(Dollar)    ASSERT_TOKEN(Hash)
    ASSERT_TOKEN(LParen)     ASSERT_TOKEN(RParen)    ASSERT_TOKEN(LBracket)  ASSERT_TOKEN(RBracket)
    ASSERT_TOKEN(LBrace)     ASSERT_TOKEN(RBrace)    ASSERT_TOKEN(Arrow)     ASSERT_TOKEN(Ellipsis)
    ASSERT_TOKEN(Scope)      ASSERT_TOKEN(Equal)

    ASSERT_TOKEN(PlusPlus)   ASSERT_TOKEN(MinusMinus) ASSERT_TOKEN(AmpAmp)          ASSERT_TOKEN(PipePipe)
    ASSERT_TOKEN(EqualEqual) ASSERT_TOKEN(LessLess)   ASSERT_TOKEN(GreaterGreater)

    ASSERT_TOKEN(PlusAssign)    ASSERT_TOKEN(MinusAssign)   ASSERT_TOKEN(StarAssign)    ASSERT_TOKEN(SlashAssign)
    ASSERT_TOKEN(PercentAssign) ASSERT_TOKEN(CaretAssign)   ASSERT_TOKEN(TildeAssign)   ASSERT_TOKEN(NotEqual)
    ASSERT_TOKEN(AmpAssign)     ASSERT_TOKEN(PipeAssign)    ASSERT_TOKEN(LessEqual)     ASSERT_TOKEN(GreaterEqual)
    ASSERT_TOKEN(ColonAssign)

    ASSERT_TOKEN(LessLessAssign) ASSERT_TOKEN(GreaterGreaterAssign) ASSERT_TOKEN(EqualEqualEqual)

    ASSERT_TOKEN(Backslash) ASSERT_TOKEN(NewLine)

    ASSERT_TOKEN(EndFile)
}

void TestLexerStrings() {
    const std::string 
    source =    "\"Hello, World!\""
                "\"Hello, World!\n\""
                "\"Hello, World!\t\""
                "\"Hello, World!\\\"\""
                "\"Hello, World!\\\\\""
    ;

    Lexer lexer(source);

    ASSERT_TOKEN_VALUE(String, "Hello, World!")
    ASSERT_TOKEN_VALUE(String, "Hello, World!\n")
    ASSERT_TOKEN_VALUE(String, "Hello, World!\t")
    ASSERT_TOKEN_VALUE(String, "Hello, World!\"")
    ASSERT_TOKEN_VALUE(String, "Hello, World!\\")
    ASSERT_TOKEN(EndFile)
}

void TestLexerIdentifiers() {
    const std::string 
    source =    "a b c d e f g h i j k l m n o p q r s t u v w x y z "
                "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z "
                "_ _1 _2 _3 _4 _5 _6 _7 _8 _9 _0 "
                "a1 a2 a3 a4 a5 a6 a7 a8 a9 a0 "
                "A1 A2 A3 A4 A5 A6 A7 A8 A9 A0 "
                "a_ a_1 a_2 a_3 a_4 a_5 a_6 a_7 a_8 a_9 a_0 "
                "A_ A_1 A_2 A_3 A_4 A_5 A_6 A_7 A_8 A_9 A_0 "
    ;

    Lexer lexer(source);

    for (int i = 0; i < 105; i++) { ASSERT_TOKEN(Identifier) }
    
    ASSERT_TOKEN(EndFile)
}
