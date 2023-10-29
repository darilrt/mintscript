#include "sa_symbol.h"

sa::Type::Type(const std::string &name) {
    this->name = name;
}

void sa::Type::SetMethod(std::string name, sa::Method symbol) {
    methods[name] = symbol;
    methods[name].name = name;
}

sa::Method *sa::Type::GetMethod(std::string name)  {
    if (methods.find(name) != methods.end()) {
        return &methods[name];
    }
    return nullptr;
}

bool sa::Type::HasMethod(std::string name) {
    return methods.find(name) != methods.end();
}

void sa::Type::AddField(std::string name, sa::Field symbol) {
    fields[name] = symbol;
    fields[name].name = name;
}

void sa::Type::SetField(std::string name, sa::Field symbol) {
    fields[name] = symbol;
    fields[name].name = name;
    fields[name].offset = lastFieldOffset++;
}

sa::Field *sa::Type::GetField(std::string name) {
    if (fields.find(name) != fields.end()) {
        return &fields[name];
    }
    return nullptr;
}

bool sa::Type::HasField(std::string name) {
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

sa::Type *sa::SymbolTable::GetType(std::string name) {
    if (types.find(name) != types.end()) {
        return &types[name];
    } else if (parent != nullptr) {
        return parent->GetType(name);
    }
    return nullptr;
}

sa::Field::Field(bool isMutable, Type *type)  {
    this->isMutable = isMutable;
    this->type = type;
}