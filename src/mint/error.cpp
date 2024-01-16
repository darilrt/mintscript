#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "mint/error.hpp"

void Error::warn(const std::string &message, const Location &location)
{
    std::cout << "\033[1m\033[32mWarning\033[0m at \033[36m" << location.to_string() << "\033[0m\n";
    std::cout << message << "\n";
}

void Error::error(const std::string &message, const Location &location)
{
    std::cout << "\033[1m\033[31mError\033[0m at \033[36m" << location.to_string() << "\033[0m\n";
    std::cout << message << "\n";
    exit(1);
}

void Error::error_in_line(const std::string &message, const Token &token, std::ifstream &file)
{
    Location location = token.location;
    file.seekg(0);
    std::string line;
    int line_number = 1;
    std::cerr << "\033[1m\033[31mError\033[0m at \033[36m" << location.to_string() << "\033[0m\n";
    while (std::getline(file, line))
    {
        if (line_number == location.line)
        {
            std::string prefix = " " + std::to_string(location.line) + " | ";
            std::cerr << prefix << line << "\n";
            for (int i = 0; i < prefix.size(); i++)
                std::cerr << " ";
            for (int i = 0; i < location.column - 1; i++)
                std::cerr << " ";
            for (int i = 0; i < token.value.size(); i++)
                std::cerr << "\033[1m\033[31m^\033[0m";
            std::cerr << "\n";
            break;
        }
        line_number++;
    }
    std::cerr << message << "\n";
    exit(1);
}