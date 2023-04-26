#pragma once

#include "ast.h"

class Eval {
public:
    Eval(ASTNode* ast) : ast(ast) { }

    mObject* Evaluate();

private:
    ASTNode* ast;
};