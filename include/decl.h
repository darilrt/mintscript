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
    ASTNode* declaration;
    ASTNode* expression;
    
    AssignmentAST(Token type, ASTNode* declaration, ASTNode* expression) : type(type), declaration(declaration), expression(expression) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class VarDeclarationAST : public ASTNode {
public:
    bool isMutable;
    Token identifier;
    ASTNode* type;
    ASTNode* expression;

    VarDeclarationAST(bool isMutable, Token identifier, ASTNode* type, ASTNode* expression) : isMutable(isMutable), identifier(identifier), type(type), expression(expression) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class LambdaAST : public ASTNode {
public:
    std::vector<ASTNode*> parameters;
    ASTNode* returnType;
    ASTNode* body;

    LambdaAST(std::vector<ASTNode*> parameters, ASTNode* returnType, ASTNode* body) : parameters(parameters), returnType(returnType), body(body) { }

    mList Accept(Visitor* visitor) override { return visitor->Visit(this); }
};