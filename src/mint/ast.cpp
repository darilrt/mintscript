#include <string>
#include "mint/ast.hpp"

Ast::~Ast()
{
    switch (type)
    {
    case Integer:
        delete (int *)value;
        break;
    case Float:
        delete (float *)value;
        break;
    case String:
        delete (std::string *)value;
        break;
    case Boolean:
        delete (bool *)value;
        break;
    case Identifier:
        delete (std::string *)value;
        break;
    }
}

std::string Ast::to_string() const
{
    switch (type)
    {
    case Integer:
        return std::to_string(*(int *)value);
    case Float:
        return std::to_string(*(float *)value);
    case String:
        return "\"" + *(std::string *)value + "\"";
    case Boolean:
        return *(bool *)value ? "true" : "false";
    case Identifier:
        return *(std::string *)value;
    default:
        return "None";
    }
}