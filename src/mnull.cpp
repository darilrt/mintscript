#include "mnull.h"
#include "mstr.h"
#include "mfn.h"

mType* mnull::Type = new mType(
    "null",
    []() -> void {
        mnull::Type->methods["ToString"] = new zFunction(&mnull::ToString);
    },
    []() -> mObject* {
        return mnull::Null;
    }
);

mnull* mnull::Null = new mnull();

mnull::mnull() : mObject(mnull::Type) { }

std::string mnull::ToString() {
    return "null";
}

mObject *mnull::ToString(mObject *_args, mObject *_kwargs, mObject *_self) {
    return new mStr("null");
}

void mnull::Release() { }
