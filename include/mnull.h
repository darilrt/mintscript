#pragma once

#include "object.h"
#include "types.h"

#include <string>

class mNull : public mObject {
public:
    static mType* Type;
    static mNull* Null;

    mNull();

    std::string ToString();

    static mObject* ToString(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mEquals(mObject* args, mObject* kwargs, mObject *_self);

    void Release();

private:
    mNull(mType* type) : mObject(type) { }
};