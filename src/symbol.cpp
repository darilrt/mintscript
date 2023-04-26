#include "symbol.h"

zSymbolTable *zSymbolTable::globals = new zSymbolTable();
zSymbolTable *zSymbolTable::locals = zSymbolTable::globals;

void zSymbolTable::Add(std::string name, mObject *value) {
    table[name] = value;
}

mObject *zSymbolTable::Get(std::string name) {
    return table[name];
}

mObject **zSymbolTable::GetRef(std::string name) {
    return &table[name];
}

void zSymbolTable::Set(std::string name, mObject *value) {
    for (auto it = table.begin(); it != table.end(); ++it) {
        if (it->first == name) {
            it->second = value;
            return;
        }
    }

    Add(name, value);
}