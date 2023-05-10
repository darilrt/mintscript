#include "MintScript.h"
#include "expr.h"
#include "token.h"

#include <iostream>

PropertyExprAST::~PropertyExprAST() {
    if (next != nullptr) {
        delete next;
    }
}

IndexExprAST::~IndexExprAST() {
    if (expr != nullptr) {
        delete expr;
    }

    if (index != nullptr) {
        delete index;
    }
}

CallExprAST::~CallExprAST() {
    if (property != nullptr) {
        delete property;
    }
}

UnaryExprAST::~UnaryExprAST() {
    if (expr != nullptr) {
        delete expr;
    }
}

BinaryExprAST::~BinaryExprAST() {
    if (lhs != nullptr) {
        delete lhs;
    }

    if (rhs != nullptr) {
        delete rhs;
    }
}

TernaryExprAST::~TernaryExprAST() {
    if (condition != nullptr) {
        delete condition;
    }

    if (trueExpr != nullptr) {
        delete trueExpr;
    }

    if (falseExpr != nullptr) {
        delete falseExpr;
    }
}

ParenExprAST::~ParenExprAST() {
    if (expr != nullptr) {
        delete expr;
    }
}
