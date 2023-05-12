#include "mfn.h"

mFunction::mFunction() : mObject(mFunctionType) { }

mFunction::mFunction(mObject *(*func)(mObject *args, mObject *kwargs, mObject *self)) : mObject(mFunctionType) {
    this->func = func;
}

std::string mFunction::ToString() {
    return "<function>";
}

mObject *mFunction::Call(mObject *args, mObject *kwargs, mObject *self) {
    return func(args, kwargs, self);
}
