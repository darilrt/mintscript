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
    
    AstVisitor();

    ~AstVisitor();

    static AstVisitor* Eval(ASTNode* node);

    sa::Symbol Visit(ASTNode* node);

    sa::Symbol Visit(ProgramAST* node);

    sa::Symbol Visit(NumberExprAST* node);

    sa::Symbol Visit(StringExprAST* node);

    sa::Symbol Visit(BoolExprAST* node);

    sa::Symbol Visit(NullExprAST* node);

    sa::Symbol Visit(PropertyExprAST* node);

    sa::Symbol Visit(IndexExprAST* node);

    sa::Symbol Visit(CallExprAST* node);

    sa::Symbol Visit(UnaryExprAST* node);

    sa::Symbol Visit(BinaryExprAST* node);

    sa::Symbol Visit(TernaryExprAST* node);

    sa::Symbol Visit(ParenExprAST* node);

    sa::Symbol Visit(ArrayExprAST* node);

    sa::Symbol Visit(AccessExprAST* node);

    sa::Symbol Visit(AssignmentAST* node);

    sa::Symbol Visit(VarDeclarationAST* node);

    sa::Symbol Visit(LambdaAST* node);

    sa::Symbol Visit(ArgDeclAST* node);

    sa::Symbol Visit(BlockAST* node);

    sa::Symbol Visit(ReturnAST* node);

    sa::Symbol Visit(FunctionAST* node);

    sa::Symbol Visit(IfAST* node);

    sa::Symbol Visit(WhileAST* node);

    sa::Symbol Visit(ForAST* node);

    sa::Symbol Visit(BreakAST* node);

    sa::Symbol Visit(ContinueAST* node);

    sa::Symbol Visit(ImportAST* node);

    sa::Symbol Visit(ExportAST* node);

    sa::Symbol Visit(ClassAST* node);

    sa::Symbol Visit(TypeSignatureAST* node);

    sa::Symbol Visit(TypeAccessAST* node);
};