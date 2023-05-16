#include "mbool.h"
#include "mstr.h"
#include "mfn.h"
#include "mlist.h"

mType* mBool::Type = new mType(
    "bool",
    []() -> void {
        mBool::Type->methods["ToString"] = new mFunction(&mBool::ToString);
    },
    []() -> mObject* {
        return mBool::False;
    }
);

mBool* mBool::True = new mBool(true);
mBool* mBool::False = new mBool(false);

mBool::mBool(bool value) : mObject(mBool::Type) {
    this->value = value;
}

std::string mBool::ToString() {
    return this->value ? "true" : "false";
}

mObject *mBool::ToString(mObject *_args, mObject *_kwargs, mObject *_self) {
    mBool* self = (mBool*) _self;
    return new mStr(self->ToString());
}

void mBool::Release() { }
