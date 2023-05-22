#pragma once

#include "object.h"
#include "types.h"

#include <string>

class mStr : public mObject {
public:
    static mType* Type;

    std::string value;

    mStr();
    mStr(std::string value);

    std::string ToString();

    static mObject* ToString(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mAdd(mObject* args, mObject* kwargs, mObject *_self);
};