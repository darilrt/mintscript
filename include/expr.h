#pragma once

#include "ast.h"
#include "token.h"

#include <string>
#include <vector>

class NumberExprAST : public ASTNode {
public:
    enum class Type {
        Int,
        Float
    };

    Type type;
    float value;

    NumberExprAST(float value) : type(Type::Float), value(value) {}
    NumberExprAST(int value) : type(Type::Int), value(value) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class StringExprAST : public ASTNode {
public:
    std::string value;

    StringExprAST(const std::string& value) : value(value) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class BoolExprAST : public ASTNode {
public:
    bool value;

    BoolExprAST(bool value) : value(value) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class NullExprAST : public ASTNode {
public:
    NullExprAST() = default;

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class LambdaExprAST : public ASTNode {
public:
    LambdaExprAST() = default;

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class PropertyExprAST : public ASTNode {
public:
    std::string name;

    ~PropertyExprAST();

    PropertyExprAST(const std::string& name) : name(name) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class IndexExprAST : public ASTNode {
public:
    ASTNode* expr = nullptr;
    ASTNode* index = nullptr;

    ~IndexExprAST();

    IndexExprAST(ASTNode* expr, ASTNode* index) : expr(expr), index(index) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class CallExprAST : public ASTNode {
public:
    ASTNode* property = nullptr;
    std::vector<ASTNode*> args;

    ~CallExprAST();

    CallExprAST(ASTNode* property) : property(property) {}

    CallExprAST(ASTNode* property, std::vector<ASTNode*> args) : property(property), args(args) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class UnaryExprAST : public ASTNode {
public:
    Token op;
    ASTNode* expr = nullptr;
    bool isPrefix;

    ~UnaryExprAST();

    UnaryExprAST(Token op, ASTNode* expr) : op(op), expr(expr), isPrefix(true) {}
    UnaryExprAST(Token op, ASTNode* expr, bool isPrefix) : op(op), expr(expr), isPrefix(isPrefix) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class BinaryExprAST : public ASTNode {
public:
    Token op;
    ASTNode* lhs = nullptr;
    ASTNode* rhs = nullptr;

    ~BinaryExprAST();

    BinaryExprAST(Token op, ASTNode* lhs, ASTNode* rhs) : op(op), lhs(lhs), rhs(rhs) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class TernaryExprAST : public ASTNode {
public:
    ASTNode* condition = nullptr;
    ASTNode* trueExpr = nullptr;
    ASTNode* falseExpr = nullptr;

    ~TernaryExprAST();

    TernaryExprAST(ASTNode* condition, ASTNode* trueExpr, ASTNode* falseExpr) : condition(condition), trueExpr(trueExpr), falseExpr(falseExpr) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ParenExprAST : public ASTNode {
public:
    ASTNode* expr = nullptr;

    ~ParenExprAST();

    ParenExprAST(ASTNode* expr) : expr(expr) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ArrayExprAST : public ASTNode {
public:
    std::vector<ASTNode*> values;

    ~ArrayExprAST();

    ArrayExprAST() {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class AccessExprAST : public ASTNode {
public:
    ASTNode* expr = nullptr;
    Token name;

    ~AccessExprAST();

    AccessExprAST(ASTNode* expr, Token name) : expr(expr), name(name) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ExportItemAST : public ASTNode {
public:
    Token name;
    ASTNode* expr = nullptr;

    ~ExportItemAST();

    ExportItemAST(Token name) : name(name) {}

    ExportItemAST(Token name, ASTNode* expr) : name(name), expr(expr) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class ImportItemAST : public ASTNode {
public:
    Token name;
    Token alias;

    ~ImportItemAST();

    ImportItemAST(Token name) : name(name) {}

    ImportItemAST(Token name, Token alias) : name(name), alias(alias) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class TypeSignatureAST : public ASTNode {
public:
    Token name;
    std::vector<ASTNode*> types;

    ~TypeSignatureAST();

    TypeSignatureAST(Token name) : name(name) {}

    TypeSignatureAST(Token name, std::vector<ASTNode*> types) : name(name), types(types) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};

class TypeAccessAST : public ASTNode {
public:
    TypeSignatureAST* lhs = nullptr;
    TypeSignatureAST* rhs = nullptr;

    ~TypeAccessAST();

    TypeAccessAST(TypeSignatureAST* lhs, TypeSignatureAST* rhs) : lhs(lhs), rhs(rhs) {}

    sa::Symbol* Accept(Visitor* visitor) override { return visitor->Visit(this); }
};