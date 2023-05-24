#pragma once

#include <vector>
#include <string>
#include <map>

#include "object.h"
#include "types.h"
#include "symbol.h"

class ASTNode;

class mFunction : public mObject {
public:
    struct Arg {
        std::string name;
        mType* type;
    };

    static mType* Type;

    std::string name = "";
    ASTNode* ast = nullptr;

    mObject* (*func)(mObject* args, mObject* kwargs, mObject *self) = nullptr;
    mType* returnType = nullptr;
    std::vector<Arg> args;
    mSymbolTable* symbolTable = nullptr;

    mFunction();
    mFunction(mObject* (*func)(mObject* args, mObject* kwargs, mObject *self));

    std::string ToString();

    mObject* Call(mObject* args, mObject* kwargs, mObject *self=nullptr);
};