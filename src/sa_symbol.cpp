#include "sa_symbol.h"

sa::Symbol::Symbol(bool isType, bool isMutable, Symbol *type)  {
    this->isType = isType;
    this->isMutable = isMutable;
    this->type = type;
}

inline void sa::Symbol::Method(std::string name, Symbol symbol) {
    if (methods == nullptr) {
        methods = new SymbolTable();
    }
    methods->Set(name, symbol);
}

inline void sa::Symbol::Field(std::string name, Symbol symbol) {
    if (fields == nullptr) {
        fields = new SymbolTable();
    }
    fields->Set(name, symbol);
}

sa::Symbol *sa::SymbolTable::Get(std::string name) {
    if (symbols.find(name) != symbols.end()) {
        return &symbols[name];
    } else if (parent != nullptr) {
        return parent->Get(name);
    }
    return nullptr;
}