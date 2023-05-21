#include "types.h"
#include "mfn.h"
#include "symbol.h"
#include "mnull.h"
#include "module.h"

mType* mType::Type = nullptr;

mType::mType(const std::string& name, void (*Init)(), mObject *(*New)()) : name(name), Init(Init), New(New), mObject(nullptr) {
    if (mType::Type == nullptr && name != "type") {
        mType::InitType();
    }

    type = mType::Type;
}

std::string mType::ToString() {
    return "<class '" + name + "'>";
}

mObject *mType::NewInstance() {
    if (New != nullptr) {
        mObject* object = New();

        for (auto field : object->type->fieldsInfo) {
            object->fields[field.first] = mNull::Null;
        }

        return object;
    }
    
    return nullptr;
}

void mType::Release() { }

void mType::SetMethod(const std::string& name, mObject *(*func)(mObject *args, mObject *kwargs, mObject *_self)) {
    methods[name] = new mFunction(func);
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

mObject *mType::mCall(mObject *args, mObject *kwargs, mObject *_self) {
    return ((mType*)_self)->NewInstance();
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

mType::mFieldInfo::mFieldInfo(std::string name, mType *type) : name(name), type(type), mObject(mType::mFieldInfo::Type) { }
