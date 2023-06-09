#pragma once

#include <iostream>
#include <string>
#include <map>

#define INCREF(x) if (x != nullptr) { x->AddRef(); }
#define DECREF(x) if (x != nullptr) { x->Release(); }

class mType;

// Base class for all objects in the language.
class mObject {
public:
    int refCount = 0; // Reference count for garbage collection.
    mType* type = nullptr; // Type of the object.
    std::map<std::string, mObject*> fields; // Attributes of the object.
    
    mObject(mType* type);

    virtual std::string ToString();

    // call the object as a function
    virtual mObject* Call(mObject* args, mObject* kwargs, mObject* self=nullptr);
    
    // call method on the object
    virtual mObject* CallMethod(std::string name, mObject* args, mObject* kwargs);
    
    // check if the object has an method
    bool HasMethod(const std::string& name);

    // get the object's method
    mObject* GetMethod(const std::string& name);

    // check if the object has an attribute
    bool HasField(const std::string& name);

    // get the object's attribute
    mObject* GetField(const std::string& name);
    
    // get the object's attribute reference
    mObject** GetFieldRef(const std::string& name);

    // set the object's attribute
    inline void SetField(const std::string& name, mObject* value) { fields[name] = value; }

    // reference counting
    void AddRef();

    virtual void Release();
};