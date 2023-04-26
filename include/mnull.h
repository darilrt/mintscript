#pragma once

#include "object.h"
#include "types.h"

#include <string>

class mnull : public mObject {
public:
    static mType* Type;
    static mnull* Null;

    mnull();

    std::string ToString();

    static mObject* ToString(mObject* args, mObject* kwargs, mObject *_self);

    void Release();
};