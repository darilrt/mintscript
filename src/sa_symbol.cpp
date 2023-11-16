#include "sa_symbol.h"

#include <iostream>

sa::SymbolTable* sa::global = new sa::SymbolTable();

sa::Type::Type() {
    name = "";
}

sa::Type::Type(const std::string &name) {
    this->name = name;
}

void sa::Type::SetMethod(std::string name, sa::Method symbol) {
    symbol.parent = this;
    symbol.offset = lastMethodOffset++;
    methods[name] = symbol;
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

sa::Type* sa::Type::GetVariant(const std::vector<sa::Type*>& types) {
    for (sa::Type* variant : variants) {
        bool match = true;

        if (variant->typeParameters.size() != types.size()) {
            continue;
        }

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

std::string sa::Type::GetFullName() {
    std::string fullName = name;

    Module* node = this->_module;
    while (node != nullptr) {
        fullName = node->name + "." + fullName;
        node = node->_module;
    }

    return fullName;
}

bool sa::Type::Implements(sa::Type *type) {
    if (this == type) {
        return true;
    }

    if (type->isInterface) {
        for (auto& method : type->methods) {
            if (!HasMethod(method.first)) {
                return false;
            }
        }

        return true;
    } else {
        return IsVariantOf(type);
    }

    return false;
}

sa::Symbol *sa::SymbolTable::GetSymbol(std::string name) {
    if (symbols.find(name) != symbols.end()) {
        return &symbols[name];
    } else if (parent != nullptr) {
        return parent->GetSymbol(name);
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

void sa::SymbolTable::SetModule(std::string name, Module symbol) {
    modules[name] = symbol;
}

sa::Module *sa::SymbolTable::GetModule(std::string name) {
    if (modules.find(name) != modules.end()) {
        return &modules[name];
    } else if (parent != nullptr) {
        return parent->GetModule(name);
    }
    return nullptr;
}

sa::Field::Field(bool isMutable, Type *type)  {
    this->isMutable = isMutable;
    this->type = type;
}

std::string sa::Field::GetFullName() {
    std::string fullName = name;

    Type* parent = this->parent;
    while (parent != nullptr) {
        fullName = parent->name + "_" + fullName;
        parent = parent->parent;
    }

    return fullName;
}

std::string sa::Module::GetFullName() {
    std::string fullName = name;

    Module* parent = this->_module;
    while (parent != nullptr) {
        fullName = parent->name + "." + fullName;
        parent = parent->_module;
    }

    return fullName;
}

sa::Symbol::Symbol(bool isMutable, std::string name, Type *type) {
    this->isMutable = isMutable;
    this->name = name;
    this->type = type;
}

std::string sa::Symbol::GetFullName() {
    std::string fullName = name;

    Module* parent = this->_module;
    while (parent != nullptr) {
        fullName = parent->name + "." + fullName;
        parent = parent->_module;
    }

    return fullName;
}

std::string sa::Method::GetFullName() {
    std::string fullName = name;

    Type* parent = this->parent;
    while (parent != nullptr) {
        fullName = parent->name + "_" + fullName;
        parent = parent->parent;
    }

    return fullName;
}
