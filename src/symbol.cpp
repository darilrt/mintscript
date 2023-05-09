#include "symbol.h"

zSymbolTable *zSymbolTable::globals = new zSymbolTable();
zSymbolTable *zSymbolTable::locals = zSymbolTable::globals;

zSymbolTable::Symbol *zSymbolTable::GetSymbol(const std::string &name) {
    auto it = table.find(name);

    if (it == table.end())
        return nullptr;

    return &it->second;
}

mObject *zSymbolTable::Get(const std::string &name)
{
    return table[name].value;
}

mObject **zSymbolTable::GetRef(const std::string &name) {
    return &table[name].value;
}

void zSymbolTable::Set(const std::string &name, mObject *value) {
    auto it = table.find(name);

    if (it != table.end()) {
        it->second.value = value;
        return;
    }

    table[name] = { name, value->type, value };
}

void zSymbolTable::Set(const std::string &name, mObject *value, mType *type, bool isMutable) {
    auto it = table.find(name);

    if (it != table.end()) {
        it->second.value = value;
        return;
    }

    table[name] = { name, type, value, isMutable };
}

bool zSymbolTable::Exists(const std::string &name, mType* type) {
    auto it = table.find(name);

    if (it == table.end() && type != nullptr)
        type = type->type;

    return it != table.end();
}
