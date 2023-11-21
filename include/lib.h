#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#endif

#include <string>

namespace lib {
#if defined(_WIN32) || defined(_WIN64)
    using handle_t = HMODULE;
#elif defined(__linux__)
    using handle_t = void*;
#endif

    class Library {
    public:
        Library() = default;
        Library(const std::string& path);
        ~Library();

        void Load(const std::string& path);
        void Unload();

        bool IsLoaded() const;

    private:
        handle_t handle_ = nullptr;
    };

} // namespace lib