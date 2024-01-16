#pragma once

#include <fstream>

#include "mint/token.hpp"

class Error
{
public:
    // warn prints the message to stdout.
    static void warn(const std::string &message, const Location &location);

    // error prints the message to stderr and exits with a non-zero exit code.
    static void error(const std::string &message, const Location &location);

    // error_in_line prints the message to stderr and exits with a non-zero exit code.
    // The message is prefixed with the line of code and a caret pointing to the location.
    static void error_in_line(const std::string &message, const Token &token, std::ifstream &file);
};