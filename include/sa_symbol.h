#pragma once

#include "ir.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <cstdint>

namespace sa {
    class SymbolTable;
    class Type;

    class Module {
    public:
        Module* _module = nullptr;
        std::string name;
        SymbolTable* symbols;

        Module() = default;
        Module(const std::string& name) { this->name = name; }

        inline std::string GetName() { return name; }

        std::string GetFullName();
    };

    class Field {
    public:
        Type* parent;
        bool isMutable;
        std::string name;
        int offset;
        Type* type;

        Field() = default;
        Field(bool isMutable, Type* type);

        inline std::string GetName() { return name; }

        std::string GetFullName();
    };

    class Method {
    public:
        Type* parent;
        std::string name;
        Type* type;
        uint32_t offset;
        SymbolTable* parameters = nullptr;

        Method() = default;
        Method(Type* type) { this->type = type; }
        Method(std::string name, Type* type) { this->name = name; this->type = type; }

        inline std::string GetName() { return name; }

        std::string GetFullName();
    };
    
    class Type {
    public:
        bool isInterface = false;
        Module* _module = nullptr;
        std::string name;
        std::unordered_map<std::string, Method> methods;
        std::unordered_map<std::string, Field> fields;
        std::vector<Type*> variants;
        std::vector<Type*> typeParameters;
        std::set<Type*> implements;
        Type* parent = nullptr;
        ir::Instruction* vtable = nullptr;

        Type();
        Type(const std::string& name);
        ~Type() = default;

        void SetMethod(std::string name, sa::Method symbol);
        
        sa::Method* GetMethod(std::string name);

        bool HasMethod(std::string name);

        void AddField(std::string name, sa::Field symbol);

        void SetField(std::string name, sa::Field symbol);
        
        sa::Field* GetField(std::string name);

        bool HasField(std::string name);

        bool IsVariantOf(Type* type);

        Type* GetVariant(const std::vector<sa::Type*>& types);

        std::string ToString();

        inline std::string GetName() { return name; }

        std::string GetFullName();

        inline uint32_t GetSize() { return lastFieldOffset + implements.size(); }

        bool Implements(Type* type);

    private:
        uint32_t lastFieldOffset = 0;
        uint32_t lastMethodOffset = 0;
    };

    class Symbol {
    public:
        Module* _module;
        bool isMutable;
        std::string name;
        Type* type;

        Symbol() = default;
        Symbol(bool isMutable, std::string name, Type* type);

        inline std::string GetName() { return name; }

        std::string GetFullName();
    };

    class SymbolTable {
    public:
        SymbolTable() = default;
        SymbolTable(SymbolTable* parent) { this->parent = parent; }

        inline void SetSymbol(std::string name, Symbol symbol) { symbols[name] = symbol; }

        Symbol* GetSymbol(std::string name);

        inline void SetParent(SymbolTable* parent) { this->parent = parent; }

        inline SymbolTable* GetParent() { return parent; }

        inline void SetType(std::string name, Type symbol) { types[name] = symbol; }

        Type* GetType(std::string name);

        Type* GetTypeVariant(std::string name, std::vector<sa::Type*> types);

        void SetModule(std::string name, Module symbol);

        Module* GetModule(std::string name);

    private:
        SymbolTable* parent = nullptr;
        std::unordered_map<std::string, Type> types;
        std::unordered_map<std::string, Symbol> symbols;
        std::unordered_map<std::string, Module> modules;
    };

    extern SymbolTable* global;
}