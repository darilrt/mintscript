#pragma once

#include <string>
#include <map>

#include "object.h"
#include "types.h"

class mFunction : public mObject {
public:
    mObject* (*func)(mObject* args, mObject* kwargs, mObject *self); // Function pointer.

    mFunction();
    mFunction(mObject* (*func)(mObject* args, mObject* kwargs, mObject *self));

    std::string ToString();

    mObject* Call(mObject* args, mObject* kwargs, mObject *self=nullptr);
};

static mType* mFunctionType = new mType(
    "function",
    []() -> void {
        // Register type in the global scope.
        mFunctionType->methods["mCall"] = new mFunction(
            [](mObject* args, mObject* kwargs, mObject* self) -> mObject* {
                // Call the function.
                return nullptr;
            }
        );
    },
    []() -> mObject* {
        mObject* obj = new mFunction();
        return obj;
    }
);