#pragma once

#include "object.h"
#include "types.h"

#include <string>

class mFloat : public mObject {
public:
    static mType* Type;

    float value;

    mFloat();
    mFloat(float value);

    std::string ToString();

    static mObject* ToString(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mAdd(mObject* args, mObject* kwargs, mObject *_self);
};