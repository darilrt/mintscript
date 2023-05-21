#pragma once

#include "object.h"
#include "types.h"

class mModule : public mObject {
public:
    static mType* Type;

    mModule();

    static mObject* ImportFile(const std::string &path);
};