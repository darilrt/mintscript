#pragma once

#include <string>
#include <vector>

class mError {
public:
    static void AddError(std::string error);
    
    static void PrintErrors();
    
    static bool HasError();

    static void ClearErrors();

private:
    static std::vector<std::string> errors;
};