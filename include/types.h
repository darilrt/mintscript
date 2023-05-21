#pragma once

#include <string>
#include <map>
#include <vector>

#include "object.h"

#define NEW(t) t->NewInstance()

// Base class for all classes objects in the language.
class mType : public mObject {
public:
    static mType* Type; // Type of the class Type.

    class mFieldInfo : mObject {
    public:
        static mType* Type;

        std::string name;
        mType* type;

        mFieldInfo();
        
        mFieldInfo(std::string name, mType* type);
    };

    std::string name; // Name of the class.
    
    void (*Init)(); // Called when the type is initialized.
    
    mObject* (*New)(); // Called to create a new instance of the class.

    std::map<std::string, mObject*> methods; // Methods of the class.
    std::map<std::string, mFieldInfo> fieldsInfo; // Fields of the class.
    
    mType(const std::string& name, void (*Init)(), mObject* (*New)());

    std::string ToString();

    mObject* NewInstance();

    void Release();

    inline void SetFieldInfo(const std::string& name, mType* type) { fieldsInfo[name] = mFieldInfo(name, type); }

    void SetMethod(const std::string& name, mObject* (*func)(mObject* args, mObject* kwargs, mObject* _self));

    static void InitType();

    static mObject* mCall(mObject* args, mObject* kwargs, mObject* _self);
};
