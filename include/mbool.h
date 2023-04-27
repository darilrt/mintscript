#pragma once

#include "object.h"
#include "types.h"

#include <string>

class mBool : public mObject {
public:
    static mType* Type;
    static mBool* True;
    static mBool* False;

    bool value;

    mBool(bool value);

    std::string ToString();

    static mObject* ToString(mObject* args, mObject* kwargs, mObject *_self);
    
    void Release();
};