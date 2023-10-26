#include "sa_symbol.h"

sa::Symbol::Symbol(bool isType, bool isMutable, Symbol *type)  {
    this->isType = isType;
    this->isMutable = isMutable;
    this->type = type;
}

void sa::Symbol::SetMethod(std::string name, sa::Method symbol) {
    methods[name] = symbol;
    methods[name].name = name;
}

sa::Method *sa::Symbol::GetMethod(std::string name)  {
    if (methods.find(name) != methods.end()) {
        return &methods[name];
    }
    return nullptr;
}

bool sa::Symbol::HasMethod(std::string name) {
    return methods.find(name) != methods.end();
}

void sa::Symbol::AddField(std::string name, sa::Field symbol) {
    fields[name] = symbol;
    fields[name].name = name;
}

void sa::Symbol::SetField(std::string name, sa::Field symbol) {
    fields[name] = symbol;
    fields[name].name = name;
    fields[name].offset = lastFieldOffset++;
}

sa::Field *sa::Symbol::GetField(std::string name) {
    if (fields.find(name) != fields.end()) {
        return &fields[name];
    }
    return nullptr;
}

bool sa::Symbol::HasField(std::string name) {
    return fields.find(name) != fields.end();
}

sa::Symbol *sa::SymbolTable::Get(std::string name) {
    if (symbols.find(name) != symbols.end()) {
        return &symbols[name];
    } else if (parent != nullptr) {
        return parent->Get(name);
    }
    return nullptr;
}

sa::Field::Field(bool isMutable, Symbol *type)  {
    this->isMutable = isMutable;
    this->type = type;
}