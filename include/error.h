#pragma once

#include <string>
#include <vector>

#define ERROR(msg) mError::AddError(\
    "DEBUG( " + std::string(__FILE__) + std::string(":") + std::to_string(__LINE__) + std::string(":1 )") + std::string(": ") \
    + msg + std::string(" "));

class mError {
public:
    static void AddError(std::string error);
    
    static void PrintErrors();
    
    static bool HasError();

    static void ClearErrors();

private:
    static std::vector<std::string> errors;
};