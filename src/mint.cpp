#include "mint.h"
#include "mfloat.h"
#include "mlist.h"
#include "mstr.h"
#include "symbol.h"

#include <sstream>
#include <cmath>

mType* mint::Type = new mType(
    "int",
    []() -> void {
        // Regist the type in the global scope
        zSymbolTable::globals->Set("int", mint::Type);
        
        // Register methods
        mint::Type->methods["ToString"] = new zFunction(&mint::ToString);

        // Binary operators.
        mint::Type->methods["zAdd"] = new zFunction(&mint::zAdd);
        mint::Type->methods["zSub"] = new zFunction(&mint::zSub);
        mint::Type->methods["zMul"] = new zFunction(&mint::zMul);
        mint::Type->methods["zDiv"] = new zFunction(&mint::zDiv);
        mint::Type->methods["zMod"] = new zFunction(&mint::zMod);

        // Unary operators.
        mint::Type->methods["zNeg"] = new zFunction(&mint::zNeg);
        mint::Type->methods["zPos"] = new zFunction(&mint::zPos);
        mint::Type->methods["zNot"] = new zFunction(&mint::zNot);
        mint::Type->methods["zPreInc"] = new zFunction(&mint::zPreInc);
        mint::Type->methods["zPreDec"] = new zFunction(&mint::zPreDec);
        mint::Type->methods["zPostInc"] = new zFunction(&mint::zPostInc);
        mint::Type->methods["zPostDec"] = new zFunction(&mint::zPostDec);
    },
    []() -> mObject* {
        return new mint();
    }
);

mint::mint() : mObject(mint::Type) {
    this->value = 0;
}

mint::mint(int value) : mObject(mint::Type) {
    this->value = value;
}

std::string mint::ToString() {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

mObject *mint::ToString(mObject *_args, mObject *_kwargs, mObject *_self) {
    return new mStr(_self->ToString());
}

mObject *mint::zAdd(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;

    mint* self = (mint*) _self;
    mObject* other = args->GetItem(0);
    
    if (other->type == mint::Type) {
        return new mint(self->value + ((mint*) other)->value);
    }

    if (other->type == mfloat::Type) {
        return new mfloat(self->value + ((mfloat*) other)->value);
    }

    return nullptr;
}

mObject *mint::zSub(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;

    mint* self = (mint*) _self;
    mObject* other = args->GetItem(0);
    
    if (other->type == mint::Type) {
        return new mint(self->value - ((mint*) other)->value);
    }

    if (other->type == mfloat::Type) {
        return new mfloat(self->value - ((mfloat*) other)->value);
    }

    return nullptr;
}

mObject *mint::zMul(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;

    mint* self = (mint*) _self;
    mObject* other = args->GetItem(0);

    if (other->type == mint::Type) {
        return new mint(self->value * ((mint*) other)->value);
    }

    if (other->type == mfloat::Type) {
        return new mfloat(self->value * ((mfloat*) other)->value);
    }

    return nullptr;
}

mObject *mint::zDiv(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;

    mint* self = (mint*) _self;
    mObject* other = args->GetItem(0);
    
    if (other->type == mint::Type) {
        return new mint(self->value / ((mint*) other)->value);
    }

    if (other->type == mfloat::Type) {
        return new mfloat(self->value / ((mfloat*) other)->value);
    }

    return nullptr;
}

mObject *mint::zMod(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;

    mint* self = (mint*) _self;
    mObject* other = args->GetItem(0);
    
    if (other->type == mint::Type) {
        return new mint(self->value % ((mint*) other)->value);
    }

    if (other->type == mfloat::Type) {
        return new mfloat(fmod(self->value, ((mfloat*) other)->value));
    }

    return nullptr;
}

mObject *mint::zNeg(mObject *args, mObject *kwargs, mObject *_self) {
    mint* self = (mint*) _self;
    return new mint(-self->value);
}

mObject *mint::zPos(mObject *args, mObject *kwargs, mObject *_self) {
    mint* self = (mint*) _self;
    return new mint(self->value);
}

mObject *mint::zNot(mObject *args, mObject *kwargs, mObject *_self) {
    mint* self = (mint*) _self;
    return new mint(!self->value);
}

mObject *mint::zPreInc(mObject *args, mObject *kwargs, mObject *_self) {
    mint* self = (mint*) _self;
    return new mint(++self->value);
}

mObject *mint::zPreDec(mObject *args, mObject *kwargs, mObject *_self) {
    mint* self = (mint*) _self;
    return new mint(--self->value);
}

mObject *mint::zPostInc(mObject *args, mObject *kwargs, mObject *_self) {
    mint* self = (mint*) _self;
    return new mint(self->value++);
}

mObject *mint::zPostDec(mObject *args, mObject *kwargs, mObject *_self) {
    mint* self = (mint*) _self;
    return new mint(self->value--);
}
