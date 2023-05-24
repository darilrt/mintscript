#pragma once

#include "object.h"
#include "types.h"

#include <string>

class mFloat : public mObject {
public:
    static mType* Type;

    float value;

    mFloat();
    mFloat(float value);

    std::string ToString();

    static mObject* ToString(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mAdd(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mSub(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mMul(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mDiv(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mEq(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mNe(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mLt(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mGt(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mNeg(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mPos(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mNot(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mPreInc(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mPreDec(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mPostInc(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* mPostDec(mObject* args, mObject* kwargs, mObject *_self);
};