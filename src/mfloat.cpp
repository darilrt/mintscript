#include "mfloat.h"
#include "mint.h"
#include "mlist.h"
#include "mstr.h"

mType* mFloat::Type = new mType(
    "float",
    []() -> void {
        mFloat::Type->methods["zAdd"] = new zFunction(&mFloat::zAdd);
        mFloat::Type->methods["ToString"] = new zFunction(&mFloat::ToString);
    },
    []() -> mObject* {
        return new mFloat();
    }
);

mFloat::mFloat() : mObject(mFloat::Type) {
    this->value = 0.0f;
}

mFloat::mFloat(float value) : mObject(mFloat::Type) {
    this->value = value;
}

std::string mFloat::ToString() {
    return std::to_string(this->value);
}

mObject* mFloat::ToString(mObject* args, mObject* kwargs, mObject *_self) {
    const mList* _args = (mList*)args;

    mObject* self = _args->GetItem(0);

    return new mStr(self->ToString());
}

mObject* mFloat::zAdd(mObject* _args, mObject* _kwargs, mObject *_self) {
    const mList* args = (mList*)_args;
    const mList* kwargs = (mList*)_kwargs;

    mFloat* left = (mFloat*) args->GetItem(0);
    mObject* right = args->GetItem(1);

    if (right->type == mInt::Type) {
        return new mFloat(left->value + ((mInt*)right)->value);
    }

    if (right->type == mFloat::Type) {
        return new mFloat(left->value + ((mFloat*)right)->value);
    }

    return nullptr;
}