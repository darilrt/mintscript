#pragma once

class mList;
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
class VarDeclarationAST;
class LambdaAST;
class ArgDeclAST;
class BlockAST;
class ReturnAST;
class FunctionAST;
class IfAST;
class WhileAST;
class BreakAST;
class ContinueAST;
class ForAST;

class Visitor {
public:
    virtual mList Visit(ASTNode* node) = 0;

    // ExprASTs
    
    virtual mList Visit(NumberExprAST* node) = 0;

    virtual mList Visit(StringExprAST* node) = 0;
    
    virtual mList Visit(BoolExprAST* node) = 0;

    virtual mList Visit(NullExprAST* node) = 0;

    virtual mList Visit(LambdaExprAST* node) = 0;

    virtual mList Visit(PropertyExprAST* node) = 0;

    virtual mList Visit(IndexExprAST* node) = 0;

    virtual mList Visit(CallExprAST* node) = 0;

    virtual mList Visit(UnaryExprAST* node) = 0;

    virtual mList Visit(BinaryExprAST* node) = 0;

    virtual mList Visit(TernaryExprAST* node) = 0;

    virtual mList Visit(ParenExprAST* node) = 0;

    // DeclASTs

    virtual mList Visit(AssignmentAST* node) = 0;

    virtual mList Visit(VarDeclarationAST* node) = 0;

    virtual mList Visit(LambdaAST* node) = 0;

    virtual mList Visit(ArgDeclAST* node) = 0;

    virtual mList Visit(BlockAST* node) = 0;

    virtual mList Visit(FunctionAST* node) = 0;

    // Statements

    virtual mList Visit(ReturnAST* node) = 0;
    
    virtual mList Visit(IfAST* node) = 0;

    virtual mList Visit(WhileAST* node) = 0;

    virtual mList Visit(BreakAST* node) = 0;
};