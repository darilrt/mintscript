#pragma once

#include "sa_symbol.h"

class ASTNode;
class NumberExprAST;
class StringExprAST;
class BoolExprAST;
class NullExprAST;
class PropertyExprAST;
class IndexExprAST;
class CallExprAST;
class UnaryExprAST;
class BinaryExprAST;
class TernaryExprAST;
class ParenExprAST;
class AccessExprAST;
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
class ArrayExprAST;
class ImportAST;
class ExportAST;
class ProgramAST;
class ForAST;
class ClassAST;
class TypeSignatureAST;
class TypeAccessAST;

class Visitor {
public:
    virtual sa::Symbol* Visit(ASTNode* node) = 0;

    virtual sa::Symbol* Visit(ProgramAST* node) = 0;

    // ExprASTs
    
    virtual sa::Symbol* Visit(NumberExprAST* node) = 0;

    virtual sa::Symbol* Visit(StringExprAST* node) = 0;
    
    virtual sa::Symbol* Visit(BoolExprAST* node) = 0;

    virtual sa::Symbol* Visit(NullExprAST* node) = 0;

    virtual sa::Symbol* Visit(PropertyExprAST* node) = 0;

    virtual sa::Symbol* Visit(IndexExprAST* node) = 0;

    virtual sa::Symbol* Visit(CallExprAST* node) = 0;

    virtual sa::Symbol* Visit(UnaryExprAST* node) = 0;

    virtual sa::Symbol* Visit(BinaryExprAST* node) = 0;

    virtual sa::Symbol* Visit(TernaryExprAST* node) = 0;

    virtual sa::Symbol* Visit(ParenExprAST* node) = 0;

    virtual sa::Symbol* Visit(ArrayExprAST* node) = 0;

    virtual sa::Symbol* Visit(AccessExprAST* node) = 0;

    // DeclASTs

    virtual sa::Symbol* Visit(AssignmentAST* node) = 0;

    virtual sa::Symbol* Visit(VarDeclarationAST* node) = 0;

    virtual sa::Symbol* Visit(LambdaAST* node) = 0;

    virtual sa::Symbol* Visit(ArgDeclAST* node) = 0;

    virtual sa::Symbol* Visit(BlockAST* node) = 0;

    virtual sa::Symbol* Visit(FunctionAST* node) = 0;

    virtual sa::Symbol* Visit(ClassAST* node) = 0;
    
    // Statements

    virtual sa::Symbol* Visit(ReturnAST* node) = 0;
    
    virtual sa::Symbol* Visit(IfAST* node) = 0;

    virtual sa::Symbol* Visit(WhileAST* node) = 0;

    virtual sa::Symbol* Visit(BreakAST* node) = 0;

    virtual sa::Symbol* Visit(ContinueAST* node) = 0;

    virtual sa::Symbol* Visit(ForAST* node) = 0;

    // Modules

    virtual sa::Symbol* Visit(ImportAST* node) = 0;

    virtual sa::Symbol* Visit(ExportAST* node) = 0;

    // Types

    virtual sa::Symbol* Visit(TypeSignatureAST* node) = 0;

    virtual sa::Symbol* Visit(TypeAccessAST* node) = 0;
};