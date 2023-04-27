#pragma once

#include "ast.h"
#include "mlist.h"

class Eval {
public:
    Eval(ASTNode* ast) : ast(ast) { }

    mList Evaluate();

private:
    ASTNode* ast;
};