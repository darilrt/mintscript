#include "types.h"
#include "mfn.h"
#include "symbol.h"

mType* mType::Type = nullptr;

mType::mType(std::string name, void (*Init)(), mObject *(*New)()) : name(name), Init(Init), New(New), mObject(nullptr) {
    if (mType::Type == nullptr && name != "type") {
        mType::InitType();
    }

    type = mType::Type;
}

std::string mType::ToString()
{
    return "<class '" + name + "'>";
}

mObject *mType::NewInstance() {
    if (New != nullptr) {
        return New();
    }
    
    return nullptr;
}

void mType::Release() { }

void mType::InitType() {
    mType::Type = new mType(
        "type",
        []() {
            mType::Type->type = mType::Type;
            
            mSymbolTable::globals->Set("type", mType::Type);
            
            mType::Type->methods["mCall"] = new mFunction(&mType::mCall);
        },
        []() -> mObject* {
            return mType::Type;
        }
    );
}

mObject *mType::mCall(mObject *args, mObject *kwargs, mObject *_self) {
    return ((mType*)_self)->New();
}