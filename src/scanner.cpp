#include "scanner.h"

#include <iostream>

Scanner::Scanner(const std::string &source) : lexer(source), ignoreNewLine(false) { }

Token Scanner::Next() {
    if (current < buffer.size()) {
        current++;
    }

    if (current >= buffer.size()) {
        buffer.push_back(lexer.NextToken());
        current = buffer.size() - 1;
    }

    Token token = buffer[current];

    if (ignoreNewLine) {
        while (token.type == Token::Type::NewLine) {
            current++;
            
            if (current >= buffer.size()) {
                buffer.push_back(lexer.NextToken());
                current = buffer.size() - 1;
            }

            token = buffer[current];
        }
    }

    return token;
}

Token Scanner::Peek() {
    if (current == 0 && buffer.size() > 0) {
        return buffer[0];
    }

    if (current < buffer.size()) {
        return buffer[current];
    }

    buffer.push_back(lexer.NextToken());
    current = buffer.size() - 1;

    return buffer[current];
}

void Scanner::Reset() {
    current = 0;
    Peek();
}

// Reset the buffer and consume the current token 
void Scanner::Consume() {
    while (current > 0 && buffer[current - 1].type == Token::Type::NewLine) {
        current--;
    }

    buffer.erase(buffer.begin(), buffer.begin() + current);
    current = 0;
}

void Scanner::ShowBuffer() {
    std::cout << "Buffer: [";
    int i = 0;
    for (Token token : buffer) {
        if (current == i) {
            std::cout << ">";
        }
        std::cout << token.ToString() << ", ";
        i++;
    }
    std::cout << "]" << std::endl;
}
