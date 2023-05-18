#pragma once

#include "object.h"
#include "mfn.h"

class mMethodWrapper : public mObject {
public:
    static mType* Type;
    
    mObject* self = nullptr;
    mFunction* func = nullptr;
    mType* type;

    mMethodWrapper();

    std::string ToString();

    mObject* Call(mObject* args, mObject* kwargs, mObject *self=nullptr);

    static mObject* mCall(mObject* args, mObject* kwargs, mObject *self);
};