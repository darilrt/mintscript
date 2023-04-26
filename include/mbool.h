#pragma once

#include "object.h"
#include "types.h"

#include <string>

class mbool : public mObject {
public:
    static mType* Type;
    static mbool* True;
    static mbool* False;

    bool value;

    mbool(bool value);

    std::string ToString();

    static mObject* ToString(mObject* args, mObject* kwargs, mObject *_self);
    
    void Release();
};