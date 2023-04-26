#pragma once

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
    virtual mObject* Visit(ASTNode* node) = 0;

    // ExprASTs
    
    virtual mObject* Visit(NumberExprAST* node) = 0;

    virtual mObject* Visit(StringExprAST* node) = 0;

    virtual mObject* Visit(BoolExprAST* node) = 0;

    virtual mObject* Visit(NullExprAST* node) = 0;

    virtual mObject* Visit(LambdaExprAST* node) = 0;

    virtual mObject* Visit(PropertyExprAST* node) = 0;

    virtual mObject* Visit(IndexExprAST* node) = 0;

    virtual mObject* Visit(CallExprAST* node) = 0;

    virtual mObject* Visit(UnaryExprAST* node) = 0;

    virtual mObject* Visit(BinaryExprAST* node) = 0;

    virtual mObject* Visit(TernaryExprAST* node) = 0;

    virtual mObject* Visit(ParenExprAST* node) = 0;

    // DeclASTs

    virtual mObject* Visit(AssignmentAST* node) = 0;
};