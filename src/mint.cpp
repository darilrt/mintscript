#include "mint.h"
#include "mfloat.h"
#include "mlist.h"
#include "mstr.h"
#include "symbol.h"

#include <sstream>
#include <cmath>

mType* mInt::Type = new mType(
    "int",
    []() -> void {
        // Regist the type in the global scope
        zSymbolTable::globals->Set("int", mInt::Type);
        
        // Register methods
        mInt::Type->methods["ToString"] = new zFunction(&mInt::ToString);

        // Binary operators.
        mInt::Type->methods["zAdd"] = new zFunction(&mInt::zAdd);
        mInt::Type->methods["zSub"] = new zFunction(&mInt::zSub);
        mInt::Type->methods["zMul"] = new zFunction(&mInt::zMul);
        mInt::Type->methods["zDiv"] = new zFunction(&mInt::zDiv);
        mInt::Type->methods["zMod"] = new zFunction(&mInt::zMod);

        // Unary operators.
        mInt::Type->methods["zNeg"] = new zFunction(&mInt::zNeg);
        mInt::Type->methods["zPos"] = new zFunction(&mInt::zPos);
        mInt::Type->methods["zNot"] = new zFunction(&mInt::zNot);
        mInt::Type->methods["zPreInc"] = new zFunction(&mInt::zPreInc);
        mInt::Type->methods["zPreDec"] = new zFunction(&mInt::zPreDec);
        mInt::Type->methods["zPostInc"] = new zFunction(&mInt::zPostInc);
        mInt::Type->methods["zPostDec"] = new zFunction(&mInt::zPostDec);
    },
    []() -> mObject* {
        return new mInt();
    }
);

mInt::mInt() : mObject(mInt::Type) {
    this->value = 0;
}

mInt::mInt(int value) : mObject(mInt::Type) {
    this->value = value;
}

std::string mInt::ToString() {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

mObject *mInt::ToString(mObject *_args, mObject *_kwargs, mObject *_self) {
    return new mStr(_self->ToString());
}

mObject *mInt::zAdd(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;

    mInt* self = (mInt*) _self;
    mObject* other = args->GetItem(0);
    
    if (other->type == mInt::Type) {
        return new mInt(self->value + ((mInt*) other)->value);
    }

    if (other->type == mFloat::Type) {
        return new mFloat(self->value + ((mFloat*) other)->value);
    }

    return nullptr;
}

mObject *mInt::zSub(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;

    mInt* self = (mInt*) _self;
    mObject* other = args->GetItem(0);
    
    if (other->type == mInt::Type) {
        return new mInt(self->value - ((mInt*) other)->value);
    }

    if (other->type == mFloat::Type) {
        return new mFloat(self->value - ((mFloat*) other)->value);
    }

    return nullptr;
}

mObject *mInt::zMul(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;

    mInt* self = (mInt*) _self;
    mObject* other = args->GetItem(0);

    if (other->type == mInt::Type) {
        return new mInt(self->value * ((mInt*) other)->value);
    }

    if (other->type == mFloat::Type) {
        return new mFloat(self->value * ((mFloat*) other)->value);
    }

    return nullptr;
}

mObject *mInt::zDiv(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;

    mInt* self = (mInt*) _self;
    mObject* other = args->GetItem(0);
    
    if (other->type == mInt::Type) {
        return new mInt(self->value / ((mInt*) other)->value);
    }

    if (other->type == mFloat::Type) {
        return new mFloat(self->value / ((mFloat*) other)->value);
    }

    return nullptr;
}

mObject *mInt::zMod(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;

    mInt* self = (mInt*) _self;
    mObject* other = args->GetItem(0);
    
    if (other->type == mInt::Type) {
        return new mInt(self->value % ((mInt*) other)->value);
    }

    if (other->type == mFloat::Type) {
        return new mFloat(fmod(self->value, ((mFloat*) other)->value));
    }

    return nullptr;
}

mObject *mInt::zNeg(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(-self->value);
}

mObject *mInt::zPos(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(self->value);
}

mObject *mInt::zNot(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(!self->value);
}

mObject *mInt::zPreInc(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(++self->value);
}

mObject *mInt::zPreDec(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(--self->value);
}

mObject *mInt::zPostInc(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(self->value++);
}

mObject *mInt::zPostDec(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(self->value--);
}
