#pragma once

#include <memory>
#include <vector>

class Ast
{
public:
    enum Type
    {
        None,

        // Literals
        // All literals structs have the same layout:
        // Ast { value: T }
        Integer, // T = int
        Float,   // T = float
        String,  // T = std::string
        Boolean, // T = bool

        // Identifiers
        // Ast { value: std::string }
        Identifier,
    };

    Type type;
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