#pragma once

#include "lexer.h"

#include <vector>
#include <list>

class Scanner {
public:
    Scanner(const std::string& source, const std::string& filename="");
    
    Token Next();

    Token Peek();

    void Reset();

    void Consume();

    void ShowBuffer();

    inline void IgnoreNewLine(bool ignore) { ignoreNewLine = ignore; }

    inline bool IsIgnoringNewLine() const { return ignoreNewLine; }

    inline void PushAndSetIgnoreNewLine(bool ignore) { ignoreNewLineStack.push_back(ignoreNewLine); ignoreNewLine = ignore; }

    inline void PopIgnoreNewLine() { ignoreNewLine = ignoreNewLineStack.back(); ignoreNewLineStack.pop_back(); }
    
private:
    Lexer lexer;
    std::vector<Token> buffer;
    std::size_t current = 0;
    std::list<bool> ignoreNewLineStack;

    bool ignoreNewLine = false;
};
