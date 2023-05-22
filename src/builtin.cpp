#include "builtin.h"
#include "MintScript.h"

void BuiltInInit() {
    // Types
    mException::Type->Init();
    mType::Type->Init();
    mFunction::Type->Init();
    mMethodWrapper::Type->Init();
    mInt::Type->Init();
    mStr::Type->Init();
    mFloat::Type->Init();
    mBool::Type->Init();
    mNull::Type->Init();
    mList::Type->Init();
    mModule::Type->Init();
    // zDict::Type->Init();

    // Modules
    os_init();

    mSymbolTable::globals->Set("print", new mFunction([](mObject* _args, mObject* _kwargs, mObject* _self) -> mObject* {
        const mList* args = (mList*)_args;

        for (mObject* arg : args->items) {
            if (arg == nullptr) { return nullptr; }

            if (arg->type == mStr::Type) {
                std::cout << ((mStr*)arg)->value;
            } else {
                std::cout << arg->ToString();
            }

            std::cout << " ";
        }

        std::cout << std::endl;
        return nullptr;
    }));

    mSymbolTable::globals->Set("input", new mFunction([](mObject* _args, mObject* _kwargs, mObject* _self) -> mObject* {
        const mList* args = (mList*)_args;

        for (mObject* arg : args->items) {
            if (arg == nullptr) { return nullptr; }

            if (arg->type == mStr::Type) {
                std::cout << ((mStr*)arg)->value;
            } else {
                std::cout << arg->ToString();
            }

            if (arg != args->items.back()) {
                std::cout << " ";
            }
        }

        std::string input;
        std::getline(std::cin, input);

        return new mStr(input);
    }));
}