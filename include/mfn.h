#pragma once

#include <string>
#include <map>

#include "object.h"
#include "types.h"

class zFunction : public mObject {
public:
    mObject* (*func)(mObject* args, mObject* kwargs, mObject *self); // Function pointer.

    zFunction();
    zFunction(mObject* (*func)(mObject* args, mObject* kwargs, mObject *self));

    std::string ToString();

    mObject* Call(mObject* args, mObject* kwargs, mObject *self=nullptr);
};

static mType* zFunctionType = new mType(
    "function",
    []() -> void {
        // Register type in the global scope.
        zFunctionType->methods["zCall"] = new zFunction(
            [](mObject* args, mObject* kwargs, mObject* self) -> mObject* {
                // Call the function.
                return nullptr;
            }
        );
    },
    []() -> mObject* {
        mObject* obj = new zFunction();
        return obj;
    }
);