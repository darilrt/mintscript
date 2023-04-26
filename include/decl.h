#pragma once

#include "ast.h"
#include "expr.h"
#include "token.h"
#include "object.h"

#include <list>
#include <string>

class AssignmentAST : public ASTNode {
public:
    Token::Type type;
    ASTNode* declaration;
    ASTNode* expression;

    AssignmentAST(Token::Type type, ASTNode* declaration, ASTNode* expression) : type(type), declaration(declaration), expression(expression) { }

    mObject* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};