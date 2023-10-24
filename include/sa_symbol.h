#pragma once
#include <unordered_map>
#include <string>

namespace sa {
    class SymbolTable;

    class Symbol {
    public:
        bool isType;
        bool isMutable;
        std::string name;
        Symbol* type;
        SymbolTable* fields = nullptr;
        SymbolTable* methods = nullptr;

        Symbol() = default;
        Symbol(bool isType, bool isMutable, Symbol* type);

        inline void Method(std::string name, Symbol symbol);
        inline Symbol* Method(std::string name) { return methods ? methods->Get(name) : nullptr ; }

        inline void Field(std::string name, Symbol symbol);
        inline Symbol* Field(std::string name) { return fields ? fields->Get(name) : nullptr ; }

        inline Symbol* Get(std::string name) { return fields ? fields->Get(name) : nullptr ; }
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
} // namespace sa
