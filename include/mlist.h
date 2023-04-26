#pragma once

#include "object.h"
#include "types.h"
#include "mfn.h"

#include <string>
#include <vector>

class mlist : public mObject {
public:
    static mType* Type;

    std::vector<mObject*> items;

    mlist();
    mlist(std::vector<mObject*> items);

    std::string ToString();

    inline mObject* GetItem(int index) const { return items[index]; }

    inline void SetItem(int index, mObject* item) { items[index] = item; }

    inline void Insert(int index, mObject* item) { items.insert(items.begin() + index, item); }

    static mObject* Push(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* Pop(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* Insert(mObject* args, mObject* kwargs, mObject *_self);

    static mObject* Length(mObject* args, mObject* kwargs, mObject *_self);
};