#pragma once

#include "ast.h"

class Eval {
public:
    Eval(ASTNode* ast) : ast(ast) { }

    sa::Type Evaluate();

private:
    ASTNode* ast;
};