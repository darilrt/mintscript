#pragma once

#include <memory>
#include <vector>

#include "mint/token.hpp"

class Ast
{
public:
    enum Type
    {
        None,

        // Root
        // Ast { children: [ Stmt... ] }
        Program,

        // Literals
        // Ast { value: T }
        Integer, // T = int
        Float,   // T = float
        String,  // T = std::string
        Boolean, // T = bool
        // Ast { children: [ ExprList ] }
        Array,
        // Ast { children: [ Value... ] }
        ExprList,

        // Identifiers
        // Ast { value: std::string }
        Identifier,

        // Expressions
        // Ast { children: [ Node ] }
        Expression,
        // Ast { token: Type, children: [ Lhs, Rhs ] }
        BinaryOperator,
        // Ast { token: Type, children: [ Prefix ] }
        Prefix,
        // Ast { token: Type, children: [ Factor ] }
        Postfix,
    };

    Type type;
    Token token;
    void *value = nullptr;
    std::vector<std::unique_ptr<Ast>> children;

    Ast() : type(None) {}

    Ast(Type type) : type(type) {}

    ~Ast();

    inline Ast &operator[](size_t index)
    {
        return *children[index];
    }

    inline Ast &operator[](size_t index) const
    {
        return *children[index];
    }

    std::string to_string() const;
};