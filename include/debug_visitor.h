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
    mList Visit(ASTNode* node);

    mList Visit(NumberExprAST* node);

    mList Visit(StringExprAST* node);

    mList Visit(BoolExprAST* node);

    mList Visit(NullExprAST* node);

    mList Visit(LambdaExprAST* node);

    mList Visit(PropertyExprAST* node);

    mList Visit(IndexExprAST* node);

    mList Visit(CallExprAST* node);

    mList Visit(UnaryExprAST* node);

    mList Visit(BinaryExprAST* node);

    mList Visit(TernaryExprAST* node);

    mList Visit(ParenExprAST* node);

    mList Visit(AssignmentAST* node);

    mList Visit(VarDeclarationAST* node);

    mList Visit(LambdaAST* node);

    mList Visit(ArgDeclAST* node);

    mList Visit(BlockAST* node);

    mList Visit(ReturnAST* node);

    mList Visit(FunctionAST* node);

    mList Visit(IfAST* node);

    mList Visit(WhileAST* node);

    mList Visit(BreakAST* node);
};