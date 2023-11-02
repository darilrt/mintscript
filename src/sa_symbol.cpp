#include "sa_symbol.h"

#include <iostream>

sa::Type::Type() {
    name = "";
}

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

bool sa::Type::IsVariantOf(Type *type) {
    if (this == type) {
        return true;
    }
    if (parent != nullptr) {
        return parent == type || parent->IsVariantOf(type);
    }
    return false;
}

sa::Type* sa::Type::GetVariant(std::vector<sa::Type*> types) {
    for (sa::Type* variant : variants) {
        bool match = true;
        for (int i = 0; i < variant->typeParameters.size(); i++) {
            if (variant->typeParameters[i] != types[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            return variant;
        }
    }

    sa::Type* variant = new sa::Type();
    variant->name = name;
    variant->typeParameters = types;
    variant->parent = this;
    variants.push_back(variant);

    return variant;
}

std::string sa::Type::ToString() {
    std::string str = name;
    if (typeParameters.size() > 0) {
        str += "[";
        for (int i = 0; i < typeParameters.size(); i++) {
            if (i > 0) {
                str += ", ";
            }
            str += typeParameters[i]->ToString();
        }
        str += "]";
    }
    return str;
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

sa::Type *sa::SymbolTable::GetTypeVariant(std::string name, std::vector<sa::Type*> _types) {
    if (types.find(name) != types.end()) {
        return types[name].GetVariant(_types);
    } else if (parent != nullptr) {
        return parent->GetTypeVariant(name, _types);
    }
    return nullptr;
}

sa::Field::Field(bool isMutable, Type *type)  {
    this->isMutable = isMutable;
    this->type = type;
}