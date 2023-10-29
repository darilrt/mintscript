#pragma once
#include <unordered_map>
#include <string>

namespace sa {
    class SymbolTable;
    class Type;

    class Field {
    public:
        bool isMutable;
        std::string name;
        int offset;
        Type* type;

        Field() = default;
        Field(bool isMutable, Type* type);
    };

    class Method {
    public:
        std::string name;
        Type* type;
        SymbolTable* parameters = nullptr;

        Method() = default;
        Method(Type* type) { this->type = type; }
    };

    class Type {
    public:
        std::string name;
        std::unordered_map<std::string, Method> methods;
        std::unordered_map<std::string, Field> fields;

        Type() = default;
        Type(const std::string& name);

        void SetMethod(std::string name, sa::Method symbol);
        
        sa::Method* GetMethod(std::string name);

        bool HasMethod(std::string name);

        void AddField(std::string name, sa::Field symbol);

        void SetField(std::string name, sa::Field symbol);
        
        sa::Field* GetField(std::string name);

        bool HasField(std::string name);

    private:
        int lastFieldOffset = 0;
    };

    class Symbol {
    public:
        bool isMutable;
        std::string name;
        Type* type;

        Symbol() = default;
    };

    class SymbolTable {
    public:
        SymbolTable() = default;
        SymbolTable(SymbolTable* parent) { this->parent = parent; }

        inline void SetSymbol(std::string name, Symbol symbol) { symbols[name] = symbol; }

        Symbol* Get(std::string name);

        inline void SetParent(SymbolTable* parent) { this->parent = parent; }

        inline SymbolTable* GetParent() { return parent; }

        inline void SetType(std::string name, Type symbol) { types[name] = symbol; }

        Type* GetType(std::string name);

    private:
        SymbolTable* parent = nullptr;
        std::unordered_map<std::string, Type> types;
        std::unordered_map<std::string, Symbol> symbols;
    };

}