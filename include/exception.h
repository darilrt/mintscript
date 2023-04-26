#pragma once

#include <string>

#include "types.h"
#include "object.h"

class zException : public mObject {
public:
    static mType* Type;

    std::string message;

    zException();
    zException(std::string message);

    std::string ToString();

    static mObject* ToString(mObject* _args, mObject* _kwargs, mObject* _self);

    static mObject* Raise(mObject* _args, mObject* _kwargs, mObject* _self);
};