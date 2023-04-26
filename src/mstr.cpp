#include "mstr.h"
#include "mlist.h"
#include "mfn.h"
#include "symbol.h"

mType* mStr::Type = new mType(
    "str",
    []() -> void {
        zSymbolTable::globals->Set("str", mStr::Type);

        mStr::Type->methods["ToString"] = new zFunction(&mStr::ToString);
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