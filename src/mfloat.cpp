#include "mfloat.h"
#include "mint.h"
#include "mlist.h"
#include "mstr.h"

mType* mfloat::Type = new mType(
    "float",
    []() -> void {
        mfloat::Type->methods["zAdd"] = new zFunction(&mfloat::zAdd);
        mfloat::Type->methods["ToString"] = new zFunction(&mfloat::ToString);
    },
    []() -> mObject* {
        return new mfloat();
    }
);

mfloat::mfloat() : mObject(mfloat::Type) {
    this->value = 0.0f;
}

mfloat::mfloat(float value) : mObject(mfloat::Type) {
    this->value = value;
}

std::string mfloat::ToString() {
    return std::to_string(this->value);
}

mObject* mfloat::ToString(mObject* args, mObject* kwargs, mObject *_self) {
    const mlist* _args = (mlist*)args;

    mObject* self = _args->GetItem(0);

    return new mStr(self->ToString());
}

mObject* mfloat::zAdd(mObject* _args, mObject* _kwargs, mObject *_self) {
    const mlist* args = (mlist*)_args;
    const mlist* kwargs = (mlist*)_kwargs;

    mfloat* left = (mfloat*) args->GetItem(0);
    mObject* right = args->GetItem(1);

    if (right->type == mint::Type) {
        return new mfloat(left->value + ((mint*)right)->value);
    }

    if (right->type == mfloat::Type) {
        return new mfloat(left->value + ((mfloat*)right)->value);
    }

    return nullptr;
}