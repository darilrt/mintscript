#pragma once

#include "visitor.h"
#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "error.h"
#include "ir.h"

#include <iostream>
#include <vector>
#include <stack>

class AstVisitor : public Visitor {
public:
    std::stack<ir::Instruction*> stack;
    sa::SymbolTable* table;
    std::stack<std::string> nameStack;

    void PushScope();

    void PopScope();
    
    AstVisitor();

    ~AstVisitor();

    static AstVisitor* Eval(ASTNode* node);

    sa::Type* Visit(ASTNode* node);

    sa::Type* Visit(ProgramAST* node);

    sa::Type* Visit(NumberExprAST* node);

    sa::Type* Visit(StringExprAST* node);

    sa::Type* Visit(BoolExprAST* node);

    sa::Type* Visit(NullExprAST* node);

    sa::Type* Visit(PropertyExprAST* node);

    sa::Type* Visit(IndexExprAST* node);

    sa::Type* Visit(CallExprAST* node);

    sa::Type* Visit(UnaryExprAST* node);

    sa::Type* Visit(BinaryExprAST* node);

    sa::Type* Visit(TernaryExprAST* node);

    sa::Type* Visit(ParenExprAST* node);

    sa::Type* Visit(ArrayExprAST* node);

    sa::Type* Visit(AccessExprAST* node);

    sa::Type* Visit(AssignmentAST* node);

    sa::Type* Visit(VarDeclarationAST* node);

    sa::Type* Visit(LambdaAST* node);

    sa::Type* Visit(ArgDeclAST* node);

    sa::Type* Visit(BlockAST* node);

    sa::Type* Visit(ReturnAST* node);

    sa::Type* Visit(FunctionAST* node);

    sa::Type* Visit(IfAST* node);

    sa::Type* Visit(WhileAST* node);

    sa::Type* Visit(ForAST* node);

    sa::Type* Visit(BreakAST* node);

    sa::Type* Visit(ContinueAST* node);

    sa::Type* Visit(ImportAST* node);

    sa::Type* Visit(ExportAST* node);

    sa::Type* Visit(ClassAST* node);

    sa::Type* Visit(TypeSignatureAST* node);

    sa::Type* Visit(TypeAccessAST* node);
};