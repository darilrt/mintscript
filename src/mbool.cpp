#include "mbool.h"
#include "mstr.h"
#include "mfn.h"
#include "mlist.h"

mType* mbool::Type = new mType(
    "bool",
    []() -> void {
        mbool::Type->methods["ToString"] = new zFunction(&mbool::ToString);
    },
    []() -> mObject* {
        return mbool::False;
    }
);

mbool* mbool::True = new mbool(true);
mbool* mbool::False = new mbool(false);

mbool::mbool(bool value) : mObject(mbool::Type) {
    this->value = value;
}

std::string mbool::ToString() {
    return this->value ? "true" : "false";
}

mObject *mbool::ToString(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;
    const mbool* self = (mbool*) _self;
    
    return new mStr(((mbool *) self)->ToString());
}

void mbool::Release() { }
