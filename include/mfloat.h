#pragma once

#include "object.h"
#include "types.h"

#include <string>

class mfloat : public mObject {
public:
    static mType* Type;

    float value;

    mfloat();
    mfloat(float value);

    std::string ToString();

    static mObject* ToString(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zAdd(mObject* args, mObject* kwargs, mObject *_self);
};