#pragma once

#include "object.h"
#include "types.h"

#include <string>
#include <unordered_map>

class mSymbolTable {
public:
    struct Symbol {
        std::string name;
        mType* type;
        mObject* value;
        bool isMutable;
    };

    static mSymbolTable* globals;
    static mSymbolTable* locals;

    mSymbolTable* parent = nullptr;
    std::unordered_map<std::string, Symbol> table;

    static std::unordered_map<std::string, mSymbolTable*> scopes;

    mSymbolTable();
    
    mSymbolTable(mSymbolTable* parent);
    
    Symbol* GetSymbol(const std::string &name);

    mObject* Get(const std::string &name);

    mObject** GetRef(const std::string &name);
    
    void Set(const std::string &name, mObject* value);
    
    void Set(const std::string &name, mObject* value, mType* type, bool isMutable);

    bool Exists(const std::string &name, mType* type);

    void Show();

    // Return symbol table for the local scope or parent scope respectively
    static Symbol* LocalsGetSymbol(const std::string &name);

    // Create a new symbol table 
    static mSymbolTable* NewScope(const std::string &name);

    // Get the scope with the given name
    inline static mSymbolTable* GetScope(const std::string &name) { return scopes[name]; }

    // Check if a scope exists
    inline static bool ScopeExists(const std::string &name) { return scopes.find(name) != scopes.end(); }
};