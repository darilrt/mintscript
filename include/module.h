#pragma once

#include <filesystem>

#include "object.h"
#include "types.h"

class mModule : public mObject {
public:
    static mType* Type;

    mModule();

    static mObject* ImportFile(const std::filesystem::path& path);

    inline static mObject* ImportFile(const std::string &path) { return ImportFile(std::filesystem::path(path)); }

    static mObject* Import(const std::string &module_name);

    static std::filesystem::path GetModulePath(const std::string &module_name);
};