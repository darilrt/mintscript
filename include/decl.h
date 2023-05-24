#pragma once

#include "ast.h"
#include "expr.h"
#include "token.h"
#include "object.h"
#include "mlist.h"

#include <list>
#include <string>

class AssignmentAST : public ASTNode {
public:
    Token type;
    ASTNode* declaration = nullptr;
    ASTNode* expression = nullptr;
    
    AssignmentAST(Token type, ASTNode* declaration, ASTNode* expression) : type(type), declaration(declaration), expression(expression) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class VarDeclarationAST : public ASTNode {
public:
    bool isMutable;
    Token identifier;
    ASTNode* type = nullptr;
    ASTNode* expression = nullptr;

    VarDeclarationAST(bool isMutable, Token identifier, ASTNode* type, ASTNode* expression) : isMutable(isMutable), identifier(identifier), type(type), expression(expression) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class LambdaAST : public ASTNode {
public:
    std::vector<ASTNode*> parameters;
    ASTNode* returnType = nullptr;
    ASTNode* body = nullptr;

    LambdaAST(std::vector<ASTNode*> parameters, ASTNode* returnType, ASTNode* body) : parameters(parameters), returnType(returnType), body(body) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ReturnAST : public ASTNode {
public:
    ASTNode* expression;

    ReturnAST(ASTNode* expression) : expression(expression) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ArgDeclAST : public ASTNode {
public:
    bool isMutable;
    Token identifier;
    ASTNode* type = nullptr;
    ASTNode* defaultValue = nullptr;

    ArgDeclAST(bool isMutable, Token identifier, ASTNode* type, ASTNode* defaultValue) : isMutable(isMutable), identifier(identifier), type(type), defaultValue(defaultValue) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class FunctionAST : public ASTNode {
public:
    Token name;
    LambdaAST* lambda = nullptr;

    FunctionAST(Token name, LambdaAST* lambda) : name(name), lambda(lambda) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class BlockAST : public ASTNode {
public:
    std::vector<ASTNode*> statements;

    BlockAST(std::vector<ASTNode*> statements) : statements(statements) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ProgramAST : public ASTNode {
public:
    std::vector<ASTNode*> statements;

    ProgramAST(std::vector<ASTNode*> statements) : statements(statements) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class IfAST : public ASTNode {
public:
    ASTNode* condition = nullptr;
    ASTNode* body = nullptr;
    std::vector<IfAST*> elseIfs;
    ASTNode* elseBody = nullptr;

    IfAST() { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class WhileAST : public ASTNode {
public:
    ASTNode* condition = nullptr;
    ASTNode* body = nullptr;

    WhileAST() { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class BreakAST : public ASTNode {
public:
    BreakAST() { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ContinueAST : public ASTNode {
public:
    ContinueAST() { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ForAST : public ASTNode {
public:
    ASTNode* variable = nullptr;
    ASTNode* iterable = nullptr;
    ASTNode* body = nullptr;

    ForAST() { }

    ForAST(ASTNode* variable, ASTNode* iterable, ASTNode* body) : variable(variable), iterable(iterable), body(body) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ImportAST : public ASTNode {
public:
    bool isPath = false;
    Token path;
    Token alias;
    std::vector<Token> identifiers;
    std::vector<ASTNode*> imports;

    ImportAST() { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ExportAST : public ASTNode {
public:
    std::vector<ASTNode*> list;

    ExportAST(std::vector<ASTNode*> list) : list(list) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ClassAST : public ASTNode {
public:
    Token name;
    std::vector<ASTNode*> statements;

    ClassAST(Token name, std::vector<ASTNode*> statements) : name(name), statements(statements) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};