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

mObject *mFunction::Call(mObject *args, mObject *kwargs, mObject *self) {
    if (ast != nullptr) {
        LambdaAST* lambda = (LambdaAST*) ast;
        
        mSymbolTable* symbolTable = new mSymbolTable(mSymbolTable::locals);
        mSymbolTable::locals = symbolTable;

        if (self != nullptr) {
            symbolTable->Set("self", self);
        }

        EvalVisitor visitor;
        mList ret = lambda->body->Accept(&visitor);

        delete symbolTable;
        mSymbolTable::locals = mSymbolTable::locals->parent;

        if (ret.items.size() == 1) {
            return ret.items[0];
        }
    }

    if (func != nullptr) {
        return func(args, kwargs, self);
    }

    return nullptr;
}
