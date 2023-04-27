#include "mnull.h"
#include "mstr.h"
#include "mfn.h"

mType* mNull::Type = new mType(
    "null",
    []() -> void {
        mNull::Type->methods["ToString"] = new zFunction(&mNull::ToString);
    },
    []() -> mObject* {
        return mNull::Null;
    }
);

mNull* mNull::Null = new mNull();

mNull::mNull() : mObject(mNull::Type) { }

std::string mNull::ToString() {
    return "null";
}

mObject *mNull::ToString(mObject *_args, mObject *_kwargs, mObject *_self) {
    return new mStr("null");
}

void mNull::Release() { }
