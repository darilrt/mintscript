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
    virtual sa::Type* Visit(ASTNode* node) = 0;

    virtual sa::Type* Visit(ProgramAST* node) = 0;

    // ExprASTs
    
    virtual sa::Type* Visit(NumberExprAST* node) = 0;

    virtual sa::Type* Visit(StringExprAST* node) = 0;
    
    virtual sa::Type* Visit(BoolExprAST* node) = 0;

    virtual sa::Type* Visit(NullExprAST* node) = 0;

    virtual sa::Type* Visit(PropertyExprAST* node) = 0;

    virtual sa::Type* Visit(IndexExprAST* node) = 0;

    virtual sa::Type* Visit(CallExprAST* node) = 0;

    virtual sa::Type* Visit(UnaryExprAST* node) = 0;

    virtual sa::Type* Visit(BinaryExprAST* node) = 0;

    virtual sa::Type* Visit(TernaryExprAST* node) = 0;

    virtual sa::Type* Visit(ParenExprAST* node) = 0;

    virtual sa::Type* Visit(ArrayExprAST* node) = 0;

    virtual sa::Type* Visit(AccessExprAST* node) = 0;

    // DeclASTs

    virtual sa::Type* Visit(AssignmentAST* node) = 0;

    virtual sa::Type* Visit(VarDeclarationAST* node) = 0;

    virtual sa::Type* Visit(LambdaAST* node) = 0;

    virtual sa::Type* Visit(ArgDeclAST* node) = 0;

    virtual sa::Type* Visit(BlockAST* node) = 0;

    virtual sa::Type* Visit(FunctionAST* node) = 0;

    virtual sa::Type* Visit(ClassAST* node) = 0;
    
    // Statements

    virtual sa::Type* Visit(ReturnAST* node) = 0;
    
    virtual sa::Type* Visit(IfAST* node) = 0;

    virtual sa::Type* Visit(WhileAST* node) = 0;

    virtual sa::Type* Visit(BreakAST* node) = 0;

    virtual sa::Type* Visit(ContinueAST* node) = 0;

    virtual sa::Type* Visit(ForAST* node) = 0;

    // Modules

    virtual sa::Type* Visit(ImportAST* node) = 0;

    virtual sa::Type* Visit(ExportAST* node) = 0;

    // Types

    virtual sa::Type* Visit(TypeSignatureAST* node) = 0;

    virtual sa::Type* Visit(TypeAccessAST* node) = 0;
};