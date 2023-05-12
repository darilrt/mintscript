#include "mlist.h"
#include "mint.h"
#include "mfn.h"
#include "symbol.h"

mType* mList::Type = new mType(
    "List",
    []() -> void {
        mSymbolTable::globals->Set("List", mList::Type);

        mList::Type->methods["Push"] = new mFunction(&mList::Push);
        mList::Type->methods["Pop"] = new mFunction(&mList::Pop);
        mList::Type->methods["Insert"] = new mFunction(&mList::Insert);
        mList::Type->methods["Length"] = new mFunction(&mList::Length);
    },
    []() -> mObject* {
        return new mList();
    }
);

mList::mList() : mObject(mList::Type) { }

mList::mList(std::vector<mObject*> items) : mObject(mList::Type) {
    this->items = items;

    for (mObject* item : items) {
        INCREF(item);
    }
}

void mList::Clear() {
	for (mObject* item : items) {
		DECREF(item);
	}
}

std::string mList::ToString() {
    std::string str = "[";
    for (int i = 0; i < items.size(); i++) {
        str += items[i]->ToString();
        
        if (i < items.size() - 1) {
            str += ", ";
        }
    }
    str += "]";
    return str;
}

mObject *mList::Push(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;
    mList* self = (mList*) _self;
    
    self->items.push_back(args->GetItem(0));

    INCREF(args->GetItem(0));

    return nullptr;
}

mObject *mList::Pop(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;
    const mList* kwargs = (mList*) _kwargs;
    mList* self = (mList*) _self;

    mObject* item = self->items.back();
    self->items.pop_back();

    DECREF(item);

    return item;
}

mObject *mList::Insert(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;
    const mList* kwargs = (mList*) _kwargs;
    mList* self = (mList*) _self;
    
    mInt* index = (mInt*) args->GetItem(0);
    mInt* item = (mInt*) args->GetItem(1);

    self->items.insert(self->items.begin() + index->value, item);
    return nullptr;
}

mObject *mList::Length(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mList* args = (mList*) _args;
    const mList* kwargs = (mList*) _kwargs;
    mList* self = (mList*) _self;
    
    return new mInt(((mList*) self)->items.size());
}

void mList::Release() {
    for (int i = 0; i < items.size(); i++) {
        DECREF(items[i]);
    }

    delete this;
}
