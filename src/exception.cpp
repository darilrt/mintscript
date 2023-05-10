#include "exception.h"
#include "symbol.h"
#include "mstr.h"
#include "mfn.h"

mType* mException::Type = new mType(
    "Exception",
    []() -> void {
        mSymbolTable::globals->Set("Exception", mException::Type);
        
        mException::Type->methods["ToString"] = new zFunction(&mException::ToString);

        mException::Type->methods["Raise"] = new zFunction(&mException::Raise);
    },
    []() -> mObject* {
        return new mException();
    }
);

mException::mException() : mObject(mException::Type) {
    this->message = "";
}

mException::mException(std::string message) : mObject(mException::Type) {
    this->message = message;
}

std::string mException::ToString() {
    return this->message;
}

mObject* mException::ToString(mObject* _args, mObject* _kwargs, mObject* _self) {
    return new mStr(_self->ToString());
}

mObject *mException::Raise(mObject *_args, mObject *_kwargs, mObject *_self) {
    mException* self = (mException*) _self;

    

    return nullptr;
}
