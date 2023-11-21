#include "lib.h"

#include <iostream>

lib::Library::Library(const std::string &path) {
    Load(path);
}

lib::Library::~Library() {
    Unload();
}

void lib::Library::Load(const std::string &path) {
    typedef void (*root_t)(void); 

#if defined(_WIN32) || defined(_WIN64)
    handle_ = LoadLibrary(path.c_str());

    if (!handle_) {
        Unload();
        return;
    }

    root_t root = (root_t) GetProcAddress(handle_, "mint_Root");

    if (root != NULL) {
        root();
    }
    else {
        Unload();
        return;
    }
#elif defined(__linux__)
    handle_ = dlopen(path.c_str(), RTLD_LAZY);

    if (!handle_) {
        Unload();
        return;
    }

    root_t root = (root_t) dlsym(handle_, "mint_Root");

    if (root != NULL) {
        root();
    }
    else {
        Unload();
        return;
    }
#endif
    
    return;
}

void lib::Library::Unload() {
#if defined(_WIN32) || defined(_WIN64)
    if (handle_) {
        FreeLibrary(handle_);
        handle_ = nullptr;
    }

#elif defined(__linux__)
    if (handle_) {
        dlclose(handle_);
        handle_ = nullptr;
    }
#endif
}

bool lib::Library::IsLoaded() const {
    return handle_ != nullptr;
}
