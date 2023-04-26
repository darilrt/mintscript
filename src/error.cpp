#include "error.h"

#include <iostream>

std::vector<std::string> mError::errors;

void mError::AddError(std::string error) {
    mError::errors.push_back(error);
}

void mError::PrintErrors() {
    for (std::string error : mError::errors) {
        std::cout << error << std::endl;
    }
}

bool mError::HasError() {
    return mError::errors.size() > 0;
}

void mError::ClearErrors() {
    mError::errors.clear();
}
