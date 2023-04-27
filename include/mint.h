#pragma once

#include "object.h"
#include "types.h"

#include <string>

class mInt : public mObject {
public:
    static mType* Type;

    int value;

    mInt();
    mInt(int value);
    
    std::string ToString();

    static mObject* ToString(mObject* args, mObject* kwargs, mObject *_self);

    // Binary operators.

    static mObject* zAdd(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zSub(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zMul(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zDiv(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zMod(mObject* args, mObject* kwargs, mObject *_self);

    // Unary operators.

    static mObject* zNeg(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zPos(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zNot(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zPreInc(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zPreDec(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zPostInc(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* zPostDec(mObject* args, mObject* kwargs, mObject *_self);
};