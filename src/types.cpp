#include "types.h"
#include "mfn.h"
#include "symbol.h"
#include "mnull.h"
#include "module.h"
#include "mstr.h"
#include "mlist.h"
#include "error.h"

mType* mType::Type = nullptr;
std::vector<mType*> mType::types;

mType::mType(const std::string &name) : name(name), Init(nullptr), New(nullptr), mObject(nullptr) {
    if (mType::Type == nullptr && name != "type") {
        mType::InitType();
    }

    type = mType::Type;
    id = mType::types.size();
    mType::types.push_back(this);
}

mType::mType(const std::string &name, void (*Init)(), mObject *(*New)()) : name(name), Init(Init), New(New), mObject(nullptr) {
    if (mType::Type == nullptr && name != "type") {
        mType::InitType();
    }

    type = mType::Type;
    id = mType::types.size();
    mType::types.push_back(this);
}

std::string mType::ToString() {
    return "<class '" + name + "'>";
}

mObject *mType::NewInstance(mObject* _object) {
    mType* type = nullptr;
    mObject* object = _object;

    for (auto base : bases) {
        object = base->NewInstance(object);
    }

    if (New != nullptr) {
        type = object->type;
        object = object == nullptr ? New() : object;
    }
    else {
        type = mType::types[id];
        object = object == nullptr ? new mObject(type) : object;
    }

    object->type = type;

    for (auto field : type->fieldsInfo) {
        object->fields[field.first] = field.second.defaultValue;
    }
    
    object->fields["mType"] = type;
    
    return object;
}

void mType::Release() { }

void mType::SetMethod(const std::string& name, mObject *(*func)(mObject *args, mObject *kwargs, mObject *_self)) {
    methods[name] = new mFunction(func);
}

void mType::SetMethod(const std::string &name, mObject *func) {
    methods[name] = func;
}

mObject *mType::TypeGetMethod(const std::string &name) {
    if (methods.find(name) != methods.end()) {
        return methods[name];
    }

    for (auto base : bases) {
        mObject* method = base->TypeGetMethod(name);

        if (method != nullptr) {
            return method;
        }
    }

    return nullptr;
}

bool mType::TypeHasMethod(const std::string &name) {
    if (methods.find(name) != methods.end()) {
        return true;
    }

    for (auto base : bases) {
        if (base->TypeHasMethod(name)) {
            return true;
        }
    }
    
    return false;
}

void mType::InitType() {
    mType::Type = new mType(
        "type",
        []() {
            mModule::GetModule("reflect")->SetField("Type", mType::Type);
            
            mType::Type->type = mType::Type;
            
            mType::Type->SetMethod("mCall", mType::mCall);
        },
        []() -> mObject* {
            return mType::Type;
        }
    );
}

mObject *mType::mCall(mObject *_args, mObject *kwargs, mObject *_self) {
    const mList* args = (mList*) _args;
    mObject* obj = ((mType*)_self)->NewInstance();

    if (obj->HasMethod("mInit")) {
        obj->CallMethod("mInit", _args, kwargs);
    }

    return obj;
}

mType* mType::mFieldInfo::Type = new mType(
    "FieldInfo",
    []() -> void {
        mModule::GetModule("reflect")->SetField("FieldInfo", mType::mFieldInfo::Type);
    },
    []() -> mObject* {
        return new mType::mFieldInfo("", nullptr);
    }
);

mType::mFieldInfo::mFieldInfo() : mObject(mType::mFieldInfo::Type) { }

mType::mFieldInfo::mFieldInfo(std::string name, mType *type) : name(name), ftype(type), mObject(mType::mFieldInfo::Type) { }

mType::mFieldInfo::mFieldInfo(std::string name, mType *type, mObject *defaultValue) : name(name), ftype(type), defaultValue(defaultValue), mObject(mType::mFieldInfo::Type){ }
