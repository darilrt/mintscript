#include "mstr.h"
#include "mlist.h"
#include "mfn.h"
#include "symbol.h"

mType* mStr::Type = new mType(
    "str",
    []() -> void {
        mSymbolTable::globals->Set("str", mStr::Type);

        mStr::Type->methods["ToString"] = new mFunction(&mStr::ToString);

        // operators overloading
        mStr::Type->methods["mAdd"] = new mFunction(&mStr::mAdd);
    },
    []() -> mObject* {
        return new mStr();
    }    
);

mStr::mStr() : mObject(mStr::Type) {
    this->value = "";
}

mStr::mStr(std::string value) : mObject(mStr::Type) {
    this->value = value;
}

std::string mStr::ToString() {
    return "'" + this->value + "'";
}

mObject *mStr::ToString(mObject *_args, mObject *_kwargs, mObject *_self) {
    return new mStr(_self->ToString());
}

mObject *mStr::mAdd(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList *args = (mList*)_args;
    const mStr *other = (mStr*)args->items[0];
    const mStr *self = (mStr*)_self;

    if (other->type == mStr::Type) {
        return new mStr(self->value + other->value);
    }

    return nullptr;
}
