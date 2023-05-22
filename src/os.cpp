#include "os.h"
#include "MintScript.h"

void impl_exec(const char* cmd) {
    system(cmd);
}

mObject* os_exec(mObject* _args, mObject* _kwargs, mObject* _self) {
    const mList* args = (mList*) _args;

    if (args->items.size() == 0) {
        throw "os.exec: expected at least one argument";
    }

    const mStr* path = (mStr*) args->items[0];

    if (args->items.size() == 1) {
        impl_exec(path->value.c_str());
    } else {
        std::string cmd = path->value;

        for (size_t i = 1; i < args->items.size(); i++) {
            const mStr* arg = (mStr*) args->items[i];

            cmd += " ";
            cmd += arg->value;
        }

        impl_exec(cmd.c_str());
    }

    return nullptr;
}

void os_init(void) {
    mModule* module = (mModule*) mModule::GetModule("os");

    module->SetField("exec", new mFunction(&os_exec));
}