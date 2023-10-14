#include "types.h"
#include "module.h"
#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "error.h"
#include "symbol.h"
#include "ast_visitor.h"

AstVisitor::AstVisitor() { }

AstVisitor::~AstVisitor() { }

mObject* AstVisitor::Eval(ASTNode *node, mSymbolTable *symbolTable, mModule *module) {
    AstVisitor visitor;
    visitor.module = module;
    
    mList list = node->Accept(&visitor);

    return list.items.size() == 0 ? nullptr : list[0];
}

mList AstVisitor::Visit(ASTNode *node) {
    std::cout << "ASTNode" << std::endl;
    return {};
}

mList AstVisitor::Visit(ProgramAST *node) {
    std::cout << "ProgramAST" << std::endl;
    return {};
}

mList AstVisitor::Visit(NumberExprAST *node) {
    std::cout << "NumberExprAST" << std::endl;
    return {};
}

mList AstVisitor::Visit(StringExprAST *node) {
    return {};
}

mList AstVisitor::Visit(BoolExprAST *node) {
    return {};
}

mList AstVisitor::Visit(NullExprAST *node) {
    return {};
}

mList AstVisitor::Visit(PropertyExprAST *node) {
    return {};
}

mList AstVisitor::Visit(IndexExprAST *node) {
    return {};
}

mList AstVisitor::Visit(CallExprAST *node) {
    return {};
}

mList AstVisitor::Visit(UnaryExprAST *node) {
    return {};
}

mList AstVisitor::Visit(BinaryExprAST *node) {
    return {};
}

mList AstVisitor::Visit(TernaryExprAST *node) {
    return {};
}

mList AstVisitor::Visit(ParenExprAST *node) {
    return {};
}

mList AstVisitor::Visit(ArrayExprAST *node) {
    return {};
}

mList AstVisitor::Visit(AccessExprAST *node) {
    return {};
}

mList AstVisitor::Visit(AssignmentAST *node) {
    return {};
}

mList AstVisitor::Visit(VarDeclarationAST *node) {
    return {};
}

mList AstVisitor::Visit(LambdaAST *node) {
    return {};
}

mList AstVisitor::Visit(ArgDeclAST *node) {
    return {};
}

mList AstVisitor::Visit(BlockAST *node) {
    return {};
}

mList AstVisitor::Visit(ReturnAST *node) {
    return {};
}

mList AstVisitor::Visit(FunctionAST *node) {
    return {};
}

mList AstVisitor::Visit(IfAST* node) {
    return {};
}

mList AstVisitor::Visit(WhileAST *node) {
    return {};
}

mList AstVisitor::Visit(ForAST *node) {
    return {};
}

mList AstVisitor::Visit(BreakAST *node) {
    return {};
}

mList AstVisitor::Visit(ContinueAST *node) {
    return {};
}

mList AstVisitor::Visit(ImportAST *node) {
    return {};
}

mList AstVisitor::Visit(ExportAST *node) {
    return {};
}

mList AstVisitor::Visit(ClassAST *node) {
    return {};
}

mList AstVisitor::Visit(TypeSignatureAST *node) {
    return {};
}

mList AstVisitor::Visit(TypeAccessAST *node) {
    return {};
}