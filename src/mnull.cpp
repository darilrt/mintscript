#include "mnull.h"
#include "mstr.h"
#include "mfn.h"
#include "mlist.h"
#include "mbool.h"

mType* mNull::Type = new mType(
    "NullType",
    []() -> void {
        mNull::Type->methods["ToString"] = new mFunction(&mNull::ToString);

        mNull::Type->methods["mEq"] = new mFunction(&mNull::mEquals);
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

mObject *mNull::mEquals(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*)_args;
    const mList* kwargs = (mList*)_kwargs;
    const mNull* self = (mNull*)_self;

    mObject* other = args->GetItem(0);

    if (other->type == mNull::Type) {
        return mBool::True;
    }

    return mBool::False;
}

void mNull::Release() { }
