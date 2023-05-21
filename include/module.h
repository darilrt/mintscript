#pragma once

#include <filesystem>
#include <unordered_map>
#include <string>

#include "object.h"
#include "types.h"

class mModule : public mObject {
public:
    static mType* Type;
    static std::unordered_map<std::string, mObject*> modules;

    std::string name;

    mModule();

    mModule(const std::string &name);

    static mObject* ImportFile(const std::filesystem::path& path);

    inline static mObject* ImportFile(const std::string &path) { return ImportFile(std::filesystem::path(path)); }

    static mObject* Import(const std::string &module_name);

    static mModule* NewModule(const std::string &module_name);

    static mObject* GetModule(const std::string &module_name);

    static std::filesystem::path GetModulePath(const std::string &module_name);

    std::string ToString() override { return "Module"; }
};