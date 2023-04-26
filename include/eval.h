#pragma once

#include "ast.h"
#include "mlist.h"

class Eval {
public:
    Eval(ASTNode* ast) : ast(ast) { }

    mlist Evaluate();

private:
    ASTNode* ast;
};