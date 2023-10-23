#pragma once

#include "ast.h"

class Eval {
public:
    Eval(ASTNode* ast) : ast(ast) { }

    sa::Symbol Evaluate();

private:
    ASTNode* ast;
};