#pragma once

#include <string>
#include <map>

#include "object.h"

#define NEW(t) t->NewInstance()

// Base class for all classes objects in the language.
class mType : public mObject {
public:
    static mType* Type; // Type of the class Type.

    std::string name; // Name of the class.
    
    void (*Init)(); // Called when the type is initialized.
    mObject* (*New)(); // Called to create a new instance of the class.
    std::map<std::string, mObject*> methods; // Methods of the class.

    mType(std::string name, void (*Init)(), mObject* (*New)());

    std::string ToString();

    mObject* NewInstance();

    void Release();

    static void InitType();

    static mObject* zCall(mObject* args, mObject* kwargs, mObject* _self);
};
