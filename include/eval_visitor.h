#pragma once

#include "visitor.h"
#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "error.h"
#include "symbol.h"
#include "mlist.h"
#include "module.h"

#include <iostream>

class EvalVisitor : public Visitor {
public:
    mModule* module = nullptr;
    
    EvalVisitor();

    ~EvalVisitor();

    static mObject* Eval(ASTNode* node, mSymbolTable* symbolTable, mModule* module);

    mList Visit(ASTNode* node);

    mList Visit(ProgramAST* node);

    mList Visit(NumberExprAST* node);

    mList Visit(StringExprAST* node);

    mList Visit(BoolExprAST* node);

    mList Visit(NullExprAST* node);

    mList Visit(PropertyExprAST* node);

    mList Visit(IndexExprAST* node);

    mList Visit(CallExprAST* node);

    mList Visit(UnaryExprAST* node);

    mList Visit(BinaryExprAST* node);

    mList Visit(TernaryExprAST* node);

    mList Visit(ParenExprAST* node);

    mList Visit(ArrayExprAST* node);

    mList Visit(AccessExprAST* node);

    mList Visit(AssignmentAST* node);

    mList Visit(VarDeclarationAST* node);

    mList Visit(LambdaAST* node);

    mList Visit(ArgDeclAST* node);

    mList Visit(BlockAST* node);

    mList Visit(ReturnAST* node);

    mList Visit(FunctionAST* node);

    mList Visit(IfAST* node);

    mList Visit(WhileAST* node);

    mList Visit(ForAST* node);

    mList Visit(BreakAST* node);

    mList Visit(ContinueAST* node);

    mList Visit(ImportAST* node);

    mList Visit(ExportAST* node);

    mList Visit(ClassAST* node);
};