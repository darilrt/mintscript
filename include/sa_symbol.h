#pragma once
#include <unordered_map>
#include <string>

namespace sa {
    class SymbolTable;
    class Symbol;

    class Field {
    public:
        bool isMutable;
        std::string name;
        int offset;
        Symbol* type;

        Field() = default;
        Field(bool isMutable, Symbol* type);
    };

    class Method {
    public:
        std::string name;
        Symbol* type;
        SymbolTable* parameters = nullptr;

        Method() = default;
        Method(Symbol* type) { this->type = type; }
    };

    class Symbol {
    public:
        bool isType;
        bool isMutable;
        std::string name;
        Symbol* type;
        
        std::unordered_map<std::string, Method> methods;
        std::unordered_map<std::string, Field> fields;

        Symbol() = default;
        Symbol(bool isType, bool isMutable, Symbol* type);

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

    class SymbolTable {
    public:
        SymbolTable() = default;
        SymbolTable(SymbolTable* parent) { this->parent = parent; }

        inline void Set(std::string name, Symbol symbol) { 
            symbols[name] = symbol; 
            symbols[name].name = name;
        }

        Symbol* Get(std::string name);

        inline void SetParent(SymbolTable* parent) { this->parent = parent; }

        inline SymbolTable* GetParent() { return parent; }

    private:
        SymbolTable* parent = nullptr;
        std::unordered_map<std::string, Symbol> symbols;
    };

}