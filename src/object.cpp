#include "object.h"
#include "context.h"
#include "types.h"
#include "method_wrapper.h"

mObject::mObject(mType *type) {
    this->type = type;
    this->refCount = 0;
}

std::string mObject::ToString() {
    return "object";
}

mObject *mObject::Call(mObject *args, mObject *kwargs, mObject* self) {
    std::cout << "Cannot call object as a function." << std::endl;
    return nullptr;
}

mObject *mObject::CallMethod(std::string name, mObject *args, mObject *kwargs) {
    if (type != nullptr) {
        if (type->methods.find(name) != type->methods.end()) {
            return type->methods[name]->Call(args, kwargs, this);
        }

        std::cout << "Cannot call method '" << name << "' on object of type '" << type->name << "'." << std::endl;
        return nullptr;
    }
    
    std::cout << "Cannot call method '" << name << "' on object." << std::endl;
    return nullptr;
}

bool mObject::HasMethod(const std::string& name) {
    if (type != nullptr) {
        return type->methods.find(name) != type->methods.end();
    }

    return false;
}

mObject *mObject::GetMethod(const std::string &name) {
    if (type != nullptr) {
        if (type->methods.find(name) != type->methods.end()) {
            mMethodWrapper* wrapper = new mMethodWrapper();
            wrapper->self = this;
            wrapper->func = (mFunction*) type->methods[name];
            return wrapper;
        }
    }

    return nullptr;
}

bool mObject::HasField(const std::string &name) {
    return fields.find(name) != fields.end();
}

mObject *mObject::GetField(const std::string &name) {
    if (fields.find(name) != fields.end()) {
        return fields[name];
    }

    return nullptr;
}

mObject **mObject::GetFieldRef(const std::string &name) {
    if (fields.find(name) != fields.end()) {

        return &fields[name];
    }
    
    return nullptr;
}

void mObject::AddRef() {
    refCount++;
}

void mObject::Release() {
    refCount--;

    if (refCount <= 0) {
        delete this;
    }
}
