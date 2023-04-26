#include "mfn.h"

zFunction::zFunction() : mObject(zFunctionType) { }

zFunction::zFunction(mObject *(*func)(mObject *args, mObject *kwargs, mObject *self)) : mObject(zFunctionType) {
    this->func = func;
}

std::string zFunction::ToString() {
    return "<function>";
}

mObject *zFunction::Call(mObject *args, mObject *kwargs, mObject *self) {
    return func(args, kwargs, self);
}
