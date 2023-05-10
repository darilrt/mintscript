#include "symbol.h"

mSymbolTable *mSymbolTable::globals = new mSymbolTable();
mSymbolTable *mSymbolTable::locals = mSymbolTable::globals;

mSymbolTable::Symbol *mSymbolTable::GetSymbol(const std::string &name) {
    auto it = table.find(name);

    if (it == table.end())
        return nullptr;

    return &it->second;
}

mObject *mSymbolTable::Get(const std::string &name)
{
    return table[name].value;
}

mObject **mSymbolTable::GetRef(const std::string &name) {
    return &table[name].value;
}

void mSymbolTable::Set(const std::string &name, mObject *value) {
    auto it = table.find(name);

    if (it != table.end()) {
        it->second.value = value;
        return;
    }

    table[name] = { name, value->type, value };
}

void mSymbolTable::Set(const std::string &name, mObject *value, mType *type, bool isMutable) {
    auto it = table.find(name);

    if (it != table.end()) {
        it->second.value = value;
        return;
    }

    table[name] = { name, type, value, isMutable };
}

bool mSymbolTable::Exists(const std::string &name, mType* type) {
    auto it = table.find(name);

    if (it == table.end() && type != nullptr)
        type = type->type;

    return it != table.end();
}
