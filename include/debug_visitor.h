#pragma once

#include "visitor.h"
#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "error.h"

#include "mlist.h"

#include <iostream>

class EvalVisitor : public Visitor {
public:
    mlist Visit(ASTNode* node);

    mlist Visit(NumberExprAST* node);

    mlist Visit(StringExprAST* node);

    mlist Visit(BoolExprAST* node);

    mlist Visit(NullExprAST* node);

    mlist Visit(LambdaExprAST* node);

    mlist Visit(PropertyExprAST* node);

    mlist Visit(IndexExprAST* node);

    mlist Visit(CallExprAST* node);

    mlist Visit(UnaryExprAST* node);

    mlist Visit(BinaryExprAST* node);

    mlist Visit(TernaryExprAST* node);

    mlist Visit(ParenExprAST* node);

    mlist Visit(AssignmentAST* node);
};