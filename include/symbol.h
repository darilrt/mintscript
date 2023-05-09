#pragma once

#include "object.h"
#include "types.h"

#include <string>
#include <unordered_map>

class zSymbolTable {
public:
    struct Symbol {
        std::string name;
        mType* type;
        mObject* value;
        bool isMutable;
    };

    static zSymbolTable* globals;
    static zSymbolTable* locals;

    zSymbolTable* parent = nullptr;
    std::unordered_map<std::string, Symbol> table;

    Symbol* GetSymbol(const std::string &name);

    mObject* Get(const std::string &name);

    mObject** GetRef(const std::string &name);
    
    void Set(const std::string &name, mObject* value);
    
    void Set(const std::string &name, mObject* value, mType* type, bool isMutable);

    bool Exists(const std::string &name, mType* type);
};