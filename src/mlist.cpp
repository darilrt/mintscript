#include "mlist.h"
#include "mint.h"
#include "mfn.h"
#include "symbol.h"

mType* mlist::Type = new mType(
    "List",
    []() -> void {
        zSymbolTable::globals->Set("List", mlist::Type);

        mlist::Type->methods["Push"] = new zFunction(&mlist::Push);
        mlist::Type->methods["Pop"] = new zFunction(&mlist::Pop);
        mlist::Type->methods["Insert"] = new zFunction(&mlist::Insert);
        mlist::Type->methods["Length"] = new zFunction(&mlist::Length);
    },
    []() -> mObject* {
        return new mlist();
    }
);

mlist::mlist() : mObject(mlist::Type) { }

mlist::mlist(std::vector<mObject*> items) : mObject(mlist::Type) {
    this->items = items;
}

std::string mlist::ToString() {
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

mObject *mlist::Push(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;
    const mlist* kwargs = (mlist*) _kwargs;
    mlist* self = (mlist*) _self;

    self->items.push_back(args->GetItem(0));
    return nullptr;
}

mObject *mlist::Pop(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;
    const mlist* kwargs = (mlist*) _kwargs;
    mlist* self = (mlist*) _self;

    mObject* item = self->items.back();
    self->items.pop_back();

    return item;
}

mObject *mlist::Insert(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;
    const mlist* kwargs = (mlist*) _kwargs;
    mlist* self = (mlist*) _self;
    
    mint* index = (mint*) args->GetItem(0);
    mint* item = (mint*) args->GetItem(1);

    self->items.insert(self->items.begin() + index->value, item);
    return nullptr;
}

mObject *mlist::Length(mObject *_args, mObject *_kwargs, mObject *_self) {
    const mlist* args = (mlist*) _args;
    const mlist* kwargs = (mlist*) _kwargs;
    mlist* self = (mlist*) _self;
    
    return new mint(((mlist*) self)->items.size());
}
