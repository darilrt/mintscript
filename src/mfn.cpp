#include "mfn.h"
#include "symbol.h"
#include "eval_visitor.h"

mType* mFunction::Type = new mType(
    "Function",
    []() -> void {
        mSymbolTable::globals->Set("Function", mFunction::Type);

        mFunction::Type->methods["mCall"] = new mFunction(
            [](mObject* args, mObject* kwargs, mObject* _self) -> mObject* {
                const mFunction* self = (mFunction*) _self;
                return self->func(args, kwargs, _self);
            }
        );
    },
    []() -> mObject* {
        mObject* obj = new mFunction();
        return obj;
    }
);

mFunction::mFunction() : mObject(Type) { }

mFunction::mFunction(mObject *(*func)(mObject *args, mObject *kwargs, mObject *self)) : mObject(mFunction::Type) {
    this->func = func;
}

std::string mFunction::ToString() {
    return "<function>";
}

mObject *mFunction::Call(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;

    if (ast != nullptr) {
        LambdaAST* lambda = (LambdaAST*) ast;
        
        mSymbolTable* symbolTable = new mSymbolTable(mSymbolTable::locals);
        mSymbolTable::locals = symbolTable;

        if (_self != nullptr) {
            symbolTable->Set("self", _self);
        }

        if (args->items.size() != this->args.size()) {
            ERROR("Expected " + std::to_string(this->args.size()) + " arguments, got " + std::to_string(args->items.size()) + ".");
            return nullptr;
        }

        for (int i = 0; i < args->items.size(); i++) {
            if (this->args[i].type != nullptr) {
                if (args->items[i]->type != this->args[i].type) {
                    ERROR("Expected argument " + std::to_string(i + 1) + " to be of type '" + this->args[i].type->name + "', got '" + args->items[i]->type->name + "'.");
                    return nullptr;
                }
            }

            symbolTable->Set(this->args[i].name, args->items[i]);
        }

        EvalVisitor visitor;
        mList ret = lambda->body->Accept(&visitor);

        if (mError::HasError()) { return nullptr; }

        delete symbolTable;
        mSymbolTable::locals = mSymbolTable::locals->parent;

        if (ret.items.size() == 1) {
            return ret.items[0];
        }
    }

    if (func != nullptr) {
        return func(_args, _kwargs, _self);
    }

    return nullptr;
}
