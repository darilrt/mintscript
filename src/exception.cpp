#include "exception.h"
#include "symbol.h"
#include "mstr.h"
#include "mfn.h"

mType* zException::Type = new mType(
    "Exception",
    []() -> void {
        zSymbolTable::globals->Set("Exception", zException::Type);
        
        zException::Type->methods["ToString"] = new zFunction(&zException::ToString);

        zException::Type->methods["Raise"] = new zFunction(&zException::Raise);
    },
    []() -> mObject* {
        return new zException();
    }
);

zException::zException() : mObject(zException::Type) {
    this->message = "";
}

zException::zException(std::string message) : mObject(zException::Type) {
    this->message = message;
}

std::string zException::ToString() {
    return this->message;
}

mObject* zException::ToString(mObject* _args, mObject* _kwargs, mObject* _self) {
    return new mStr(_self->ToString());
}

mObject *zException::Raise(mObject *_args, mObject *_kwargs, mObject *_self) {
    zException* self = (zException*) _self;

    

    return nullptr;
}
