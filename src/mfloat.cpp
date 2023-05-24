#include "mfloat.h"
#include "mint.h"
#include "mlist.h"
#include "mstr.h"
#include "mbool.h"

mType* mFloat::Type = new mType(
    "float",
    []() -> void {
        mSymbolTable::globals->Set("float", mFloat::Type);
        
        mFloat::Type->methods["ToString"] = new mFunction(&mFloat::ToString);

        mFloat::Type->methods["mAdd"] = new mFunction(&mFloat::mAdd);
        mFloat::Type->methods["mSub"] = new mFunction(&mFloat::mSub);
        mFloat::Type->methods["mMul"] = new mFunction(&mFloat::mMul);
        mFloat::Type->methods["mDiv"] = new mFunction(&mFloat::mDiv);

        mFloat::Type->methods["mEq"] = new mFunction(&mFloat::mEq);
        mFloat::Type->methods["mNe"] = new mFunction(&mFloat::mNe);
        mFloat::Type->methods["mLt"] = new mFunction(&mFloat::mLt);
        mFloat::Type->methods["mGt"] = new mFunction(&mFloat::mGt);

        mFloat::Type->methods["mNeg"] = new mFunction(&mFloat::mNeg);
        mFloat::Type->methods["mPos"] = new mFunction(&mFloat::mPos);
        mFloat::Type->methods["mNot"] = new mFunction(&mFloat::mNot);
        mFloat::Type->methods["mPreInc"] = new mFunction(&mFloat::mPreInc);
        mFloat::Type->methods["mPreDec"] = new mFunction(&mFloat::mPreDec);
        mFloat::Type->methods["mPostInc"] = new mFunction(&mFloat::mPostInc);
        mFloat::Type->methods["mPostDec"] = new mFunction(&mFloat::mPostDec);
    },
    []() -> mObject* {
        return new mFloat();
    }
);

mFloat::mFloat() : mObject(mFloat::Type) {
    this->value = 0.0f;
}

mFloat::mFloat(float value) : mObject(mFloat::Type) {
    this->value = value;
}

std::string mFloat::ToString() {
    return std::to_string(this->value);
}

mObject* mFloat::ToString(mObject* args, mObject* kwargs, mObject *_self) {
    const mList* _args = (mList*)args;

    mObject* self = _args->GetItem(0);

    return new mStr(self->ToString());
}

mObject* mFloat::mAdd(mObject* _args, mObject* _kwargs, mObject *_self) {
    const mList* args = (mList*)_args;

    mFloat* self = (mFloat*)_self;

    mObject* right = args->GetItem(0);

    if (right->type == mInt::Type) {
        return new mFloat(self->value + ((mInt*)right)->value);
    }

    if (right->type == mFloat::Type) {
        return new mFloat(self->value - ((mFloat*)right)->value);
    }

    return nullptr;
}

mObject *mFloat::mSub(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*)_args;

    mFloat* self = (mFloat*)_self;

    mObject* right = args->GetItem(0);

    if (right->type == mInt::Type) {
        return new mFloat(self->value - ((mInt*)right)->value);
    }

    if (right->type == mFloat::Type) {
        return new mFloat(self->value - ((mFloat*)right)->value);
    }

    return nullptr;
}

mObject *mFloat::mMul(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*)_args;

    mFloat* self = (mFloat*)_self;

    mObject* right = args->GetItem(0);

    if (right->type == mInt::Type) {
        return new mFloat(self->value * ((mInt*)right)->value);
    }

    if (right->type == mFloat::Type) {
        return new mFloat(self->value * ((mFloat*)right)->value);
    }

    return nullptr;
}

mObject *mFloat::mDiv(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*)_args;

    mFloat* self = (mFloat*)_self;

    mObject* right = args->GetItem(0);

    if (right->type == mInt::Type) {
        return new mFloat(self->value / ((mInt*)right)->value);
    }

    if (right->type == mFloat::Type) {
        return new mFloat(self->value / ((mFloat*)right)->value);
    }

    return nullptr;
}

mObject *mFloat::mEq(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*)_args;

    mFloat* self = (mFloat*)_self;

    mObject* right = args->GetItem(0);

    if (right->type == mInt::Type) {
        return self->value == ((mInt*)right)->value ? mBool::True : mBool::False;
    }

    if (right->type == mFloat::Type) {
        return self->value == ((mFloat*)right)->value ? mBool::True : mBool::False;
    }

    return nullptr;
}

mObject *mFloat::mNe(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*)_args;

    mFloat* self = (mFloat*)_self;

    mObject* right = args->GetItem(0);

    if (right->type == mInt::Type) {
        return self->value != ((mInt*)right)->value ? mBool::True : mBool::False;
    }

    if (right->type == mFloat::Type) {
        return self->value != ((mFloat*)right)->value ? mBool::True : mBool::False;
    }

    return nullptr;
}

mObject *mFloat::mLt(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*)_args;

    mFloat* self = (mFloat*)_self;

    mObject* right = args->GetItem(0);

    if (right->type == mInt::Type) {
        return self->value < ((mInt*)right)->value ? mBool::True : mBool::False;
    }

    if (right->type == mFloat::Type) {
        return self->value < ((mFloat*)right)->value ? mBool::True : mBool::False;
    }

    return nullptr;
}

mObject *mFloat::mGt(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*)_args;

    mFloat* self = (mFloat*)_self;

    mObject* right = args->GetItem(0);

    if (right->type == mInt::Type) {
        return self->value > ((mInt*)right)->value ? mBool::True : mBool::False;
    }

    if (right->type == mFloat::Type) {
        return self->value > ((mFloat*)right)->value ? mBool::True : mBool::False;
    }

    return nullptr;
}

mObject *mFloat::mNeg(mObject *_args, mObject *_kwargs, mObject *_self) {
    mFloat* self = (mFloat*)_self;

    return new mFloat(-self->value);
}

mObject *mFloat::mPos(mObject *_args, mObject *_kwargs, mObject *_self) {
    mFloat* self = (mFloat*)_self;

    return new mFloat(+self->value);
}

mObject *mFloat::mNot(mObject *_args, mObject *_kwargs, mObject *_self) {
    mFloat* self = (mFloat*)_self;

    return new mFloat(!self->value);
}

mObject *mFloat::mPreInc(mObject *_args, mObject *_kwargs, mObject *_self) {
    mFloat* self = (mFloat*)_self;

    return new mFloat(++self->value);
}

mObject *mFloat::mPreDec(mObject *_args, mObject *_kwargs, mObject *_self) {
    mFloat* self = (mFloat*)_self;

    return new mFloat(--self->value);
}

mObject *mFloat::mPostInc(mObject *_args, mObject *_kwargs, mObject *_self) {
    mFloat* self = (mFloat*)_self;

    return new mFloat(self->value++);
}

mObject *mFloat::mPostDec(mObject *_args, mObject *_kwargs, mObject *_self) {
    mFloat* self = (mFloat*)_self;

    return new mFloat(self->value--);
}