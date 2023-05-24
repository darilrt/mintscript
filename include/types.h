#pragma once

#include <string>
#include <map>
#include <vector>
#include <unordered_map>

#include "object.h"

#define NEW(t) t->NewInstance()

// Base class for all classes objects in the language.
class mType : public mObject {
public:
    static mType* Type; // Type of the class Type.
    static std::vector<mType*> types; // All types in the language.

    class mFieldInfo : mObject {
    public:
        static mType* Type;

        std::string name;
        mType* ftype;

        mFieldInfo();
        
        mFieldInfo(std::string name, mType* type);
    };

    uint32_t id; // ID of the class.
    std::string name; // Name of the class.
    
    void (*Init)(); // Called when the type is initialized.
    
    mObject* (*New)(); // Called to create a new instance of the class.

    std::map<std::string, mObject*> methods; // Methods of the class.
    std::map<std::string, mFieldInfo> fieldsInfo; // Fields of the class.
    
    mType(const std::string& name);

    mType(const std::string& name, void (*Init)(), mObject* (*New)());

    std::string ToString();

    mObject* NewInstance();

    void Release();

    inline void SetFieldInfo(const std::string& name, mType* type) { fieldsInfo[name] = mFieldInfo(name, type); }

    inline mFieldInfo* GetFieldInfo(const std::string& name) { return &fieldsInfo[name]; }

    void SetMethod(const std::string& name, mObject* (*func)(mObject* args, mObject* kwargs, mObject* _self));

    void SetMethod(const std::string& name, mObject* func);
    
    static void InitType();

    static mObject* mCall(mObject* args, mObject* kwargs, mObject* _self);
};
