#pragma once

#include "object.h"
#include "types.h"

#include <string>
#include <unordered_map>

class zSymbolTable {
public:
    static zSymbolTable* globals;
    static zSymbolTable* locals;

    zSymbolTable* parent = nullptr;
    std::unordered_map<std::string, mObject*> table;

    // add a symbol to the table
    void Add(std::string name, mObject* value);

    // get a symbol from the table or its parents
    mObject* Get(std::string name);

    mObject** GetRef(std::string name);

    // set a symbol in the table or its parents
    void Set(std::string name, mObject* value);
};