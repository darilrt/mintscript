#pragma once

#include <string>
#include <map>

#include "object.h"
#include "types.h"

class mFunction : public mObject {
public:
    static mType* Type;
    
    mObject* (*func)(mObject* args, mObject* kwargs, mObject *self); // Function pointer.

    mFunction();
    mFunction(mObject* (*func)(mObject* args, mObject* kwargs, mObject *self));

    std::string ToString();

    mObject* Call(mObject* args, mObject* kwargs, mObject *self=nullptr);
};