
#include "method_wrapper.h"

mType* mMethodWrapper::Type = new mType(
    "MethodWrapper",
    []() -> void {
        mMethodWrapper::Type->methods["mCall"] = new mFunction(&mMethodWrapper::mCall);
    },
    []() -> mObject* { return new mMethodWrapper(); }
);

mMethodWrapper::mMethodWrapper() : mObject(mMethodWrapper::Type) { }

std::string mMethodWrapper::ToString() {
    return "MethodWrapper";
}

mObject *mMethodWrapper::Call(mObject *args, mObject *kwargs, mObject *self) {
    if (func != nullptr) {
        return func->Call(args, kwargs, this->self);
    }
    return nullptr;
}

mObject *mMethodWrapper::mCall(mObject *_args, mObject *_kwargs, mObject *_self) {
    mMethodWrapper* method = ((mMethodWrapper*) _self);
    return method->Call(_args, _kwargs, method->self);
}
