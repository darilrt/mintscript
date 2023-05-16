#include "mint.h"
#include "mfloat.h"
#include "mlist.h"
#include "mstr.h"
#include "mbool.h"
#include "symbol.h"

#include <sstream>
#include <cmath>

mType* mInt::Type = new mType(
    "int",
    []() -> void {
        // Regist the type in the global scope
        mSymbolTable::globals->Set("int", mInt::Type);
        
        // Register methods
        mInt::Type->methods["ToString"] = new mFunction(&mInt::ToString);

        // Binary operators.
        mInt::Type->methods["mAdd"] = new mFunction(&mInt::mAdd);
        mInt::Type->methods["mSub"] = new mFunction(&mInt::mSub);
        mInt::Type->methods["mMul"] = new mFunction(&mInt::mMul);
        mInt::Type->methods["mDiv"] = new mFunction(&mInt::mDiv);
        mInt::Type->methods["mMod"] = new mFunction(&mInt::mMod);

        // Comparison operators.
        mInt::Type->methods["mEq"] = new mFunction(&mInt::mEq);
        mInt::Type->methods["mNe"] = new mFunction(&mInt::mNe);
        mInt::Type->methods["mLt"] = new mFunction(&mInt::mLt);
        mInt::Type->methods["mGt"] = new mFunction(&mInt::mGt);

        // Unary operators.
        mInt::Type->methods["mNeg"] = new mFunction(&mInt::mNeg);
        mInt::Type->methods["mPos"] = new mFunction(&mInt::mPos);
        mInt::Type->methods["mNot"] = new mFunction(&mInt::mNot);
        mInt::Type->methods["mPreInc"] = new mFunction(&mInt::mPreInc);
        mInt::Type->methods["mPreDec"] = new mFunction(&mInt::mPreDec);
        mInt::Type->methods["mPostInc"] = new mFunction(&mInt::mPostInc);
        mInt::Type->methods["mPostDec"] = new mFunction(&mInt::mPostDec);
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

mObject *mInt::mAdd(mObject *_args, mObject *_kwargs, mObject *_self) {
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

mObject *mInt::mSub(mObject *_args, mObject *_kwargs, mObject *_self) {
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

mObject *mInt::mMul(mObject *_args, mObject *_kwargs, mObject *_self) {
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

mObject *mInt::mDiv(mObject *_args, mObject *_kwargs, mObject *_self) {
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

mObject *mInt::mMod(mObject *_args, mObject *_kwargs, mObject *_self) {
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

mObject *mInt::mEq(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;
    const mInt* self = (mInt*) _self;
    const mInt* other = (mInt*) args->GetItem(0);
    return mBool::FromBool(self->value == other->value);
}

mObject *mInt::mNe(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;
    const mInt* self = (mInt*) _self;
    const mInt* other = (mInt*) args->GetItem(0);
    return mBool::FromBool(self->value != other->value);
}

mObject *mInt::mLt(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;
    const mInt* self = (mInt*) _self;
    const mInt* other = (mInt*) args->GetItem(0);
    return mBool::FromBool(self->value < other->value);
}

mObject *mInt::mGt(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;
    const mInt* self = (mInt*) _self;
    const mInt* other = (mInt*) args->GetItem(0);
    return mBool::FromBool(self->value > other->value);
}

mObject *mInt::mNeg(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(-self->value);
}

mObject *mInt::mPos(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(self->value);
}

mObject *mInt::mNot(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(!self->value);
}

mObject *mInt::mPreInc(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(++self->value);
}

mObject *mInt::mPreDec(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(--self->value);
}

mObject *mInt::mPostInc(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(self->value++);
}

mObject *mInt::mPostDec(mObject *args, mObject *kwargs, mObject *_self) {
    mInt* self = (mInt*) _self;
    return new mInt(self->value--);
}
