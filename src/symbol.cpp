#include "symbol.h"

mSymbolTable *mSymbolTable::globals = new mSymbolTable();
mSymbolTable *mSymbolTable::locals = mSymbolTable::globals;
std::unordered_map<std::string, mSymbolTable*> mSymbolTable::scopes = { { "global", mSymbolTable::globals } };

mSymbolTable::mSymbolTable() { }

mSymbolTable::mSymbolTable(mSymbolTable *parent) : parent(parent) { }

mSymbolTable::Symbol *mSymbolTable::GetSymbol(const std::string &name) {
    auto it = table.find(name);

    if (it == table.end())
        return nullptr;

    return &it->second;
}

mObject *mSymbolTable::Get(const std::string &name) {
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

void mSymbolTable::Show() {
    std::cout << "Symbol table: " << std::endl;

    for (auto it = table.begin(); it != table.end(); it++) {
        std::cout << it->first << " : " << it->second.value->ToString() << std::endl;
    }
}

mSymbolTable::Symbol* mSymbolTable::LocalsGetSymbol(const std::string &name) {
    mSymbolTable* table = mSymbolTable::locals;
    
    Symbol* it = table->GetSymbol(name);

    while (it == nullptr) {
        if (table->parent == nullptr)
            return nullptr;

        table = table->parent;
        it = table->GetSymbol(name);
    }
    
    if (it == nullptr)
        return nullptr;

    return it;
}

mSymbolTable *mSymbolTable::NewScope(const std::string &name) {
    mSymbolTable* table = new mSymbolTable(mSymbolTable::globals);

    mSymbolTable::scopes[name] = table;

    return table;
}
