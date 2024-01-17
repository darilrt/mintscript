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
    case None:
    {
        return "None";
    }
    case Program:
    {
        std::string result = "Program\n";
        for (auto &child : children)
        {
            result += child->to_string() + "\n";
        }
        return result;
    }
    case Integer:
    {
        return std::to_string(*(int *)value);
    }
    case Float:
    {
        return std::to_string(*(float *)value);
    }
    case String:
    {
        return "\"" + *(std::string *)value + "\"";
    }
    case Boolean:
    {
        return *(bool *)value ? "true" : "false";
    }
    case Array:
    {
        return "[" + children[0]->to_string() + "]";
    }
    case ExprList:
    {
        std::string result;
        for (auto &child : children)
        {
            result += child->to_string();

            if (&child != &children.back())
                result += ", ";
        }
        return result;
    }
    case Identifier:
    {
        return token.value;
    }
    case Expression:
    {
        return "Expression";
    }
    case BinaryOperator:
    {
        return "(" + children[0]->to_string() + ") " + token.value + " (" + children[1]->to_string() + ")";
    }
    case Prefix:
    {
        if (token.type == Token::Plus)
            return "+(" + children[0]->to_string() + ")";
        else if (token.type == Token::Minus)
            return "-(" + children[0]->to_string() + ")";
        else if (token.type == Token::Not)
            return "!(" + children[0]->to_string() + ")";
        else if (token.type == Token::Tilde)
            return "~(" + children[0]->to_string() + ")";
        else if (token.type == Token::PlusPlus)
            return "++(" + children[0]->to_string() + ")";
        else if (token.type == Token::MinusMinus)
            return "--(" + children[0]->to_string() + ")";
        else
            return "Unknown prefix";
    }
    case Postfix:
    {
        if (token.type == Token::PlusPlus)
            return "(" + children[0]->to_string() + ")++";
        else if (token.type == Token::MinusMinus)
            return "(" + children[0]->to_string() + ")--";
        else if (token.type == Token::As)
            return "(" + children[0]->to_string() + ") as " + children[1]->to_string();
        else
            return "Unknown postfix";
    }
    default:
    {
        return "Unknown";
    }
    }
}