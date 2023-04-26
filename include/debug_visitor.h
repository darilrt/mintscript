#pragma once

#include "visitor.h"
#include "ast.h"
#include "expr.h"

#include <iostream>

class EvalVisitor : public Visitor {
public:
    mObject* Visit(ASTNode* node);

    mObject* Visit(NumberExprAST* node);

    mObject* Visit(StringExprAST* node);

    mObject* Visit(BoolExprAST* node);

    mObject* Visit(NullExprAST* node);

    mObject* Visit(LambdaExprAST* node);

    mObject* Visit(PropertyExprAST* node);

    mObject* Visit(IndexExprAST* node);

    mObject* Visit(CallExprAST* node);

    mObject* Visit(UnaryExprAST* node);

    mObject* Visit(BinaryExprAST* node);

    mObject* Visit(TernaryExprAST* node);

    mObject* Visit(ParenExprAST* node);

    mObject* Visit(AssignmentAST* node);
};