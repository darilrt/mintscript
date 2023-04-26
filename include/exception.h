#pragma once

#include <string>

#include "types.h"
#include "object.h"

class mException : public mObject {
public:
    static mType* Type;

    std::string message;

    mException();
    mException(std::string message);

    std::string ToString();

    static mObject* ToString(mObject* _args, mObject* _kwargs, mObject* _self);

    static mObject* Raise(mObject* _args, mObject* _kwargs, mObject* _self);
};