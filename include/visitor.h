#pragma once

class mlist;
class mObject;
class ASTNode;
class NumberExprAST;
class StringExprAST;
class BoolExprAST;
class NullExprAST;
class LambdaExprAST;
class PropertyExprAST;
class IndexExprAST;
class CallExprAST;
class UnaryExprAST;
class BinaryExprAST;
class TernaryExprAST;
class ParenExprAST;
class AssignmentAST;

class Visitor {
public:
    virtual mlist Visit(ASTNode* node) = 0;

    // ExprASTs
    
    virtual mlist Visit(NumberExprAST* node) = 0;

    virtual mlist Visit(StringExprAST* node) = 0;
    
    virtual mlist Visit(BoolExprAST* node) = 0;

    virtual mlist Visit(NullExprAST* node) = 0;

    virtual mlist Visit(LambdaExprAST* node) = 0;

    virtual mlist Visit(PropertyExprAST* node) = 0;

    virtual mlist Visit(IndexExprAST* node) = 0;

    virtual mlist Visit(CallExprAST* node) = 0;

    virtual mlist Visit(UnaryExprAST* node) = 0;

    virtual mlist Visit(BinaryExprAST* node) = 0;

    virtual mlist Visit(TernaryExprAST* node) = 0;

    virtual mlist Visit(ParenExprAST* node) = 0;

    // DeclASTs

    virtual mlist Visit(AssignmentAST* node) = 0;
};