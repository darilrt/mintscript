#include "scanner.h"
#include "test.h"

#include <cassert>
#include <iostream>

void TestScanner() {
    const std::string source = "mut\na = 1\n";
    
    Scanner scanner(source);
    
    scanner.PushAndSetIgnoreNewLine(true);

    assert(scanner.Next().type == Token::Type::Mut);
    assert(scanner.Next().type == Token::Type::Identifier);
    assert(scanner.Next().type == Token::Type::Equal);

    scanner.PopIgnoreNewLine();

    scanner.Consume();

    assert(scanner.Peek().type == Token::Type::Equal);
    assert(scanner.Next().type == Token::Type::Int);
    assert(scanner.Next().type == Token::Type::NewLine);
    
    scanner.Reset();

    assert(scanner.Peek().type == Token::Type::Equal);
    
    scanner.Reset();

    assert(scanner.Next().type == Token::Type::Int);
    assert(scanner.Next().type == Token::Type::NewLine);
    assert(scanner.Next().type == Token::Type::EndFile);

    scanner.Consume();

    assert(scanner.Peek().type == Token::Type::NewLine);
    assert(scanner.Next().type == Token::Type::EndFile);
}