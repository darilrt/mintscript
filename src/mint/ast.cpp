#include <string>
#include "mint/ast.hpp"

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
    case Float:
    case String:
    case Boolean:
    case Null:
    case Identifier:
    {
        return token.value;
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
    case Call:
    {
        std::string result = children[0]->to_string() + "(";
        for (auto &child : children[1]->children)
        {
            result += child->to_string();

            if (&child != &children[1]->children.back())
                result += ", ";
        }
        result += ")";
        return result;
    }
    case Subscript:
    {
        if (children.size() <= 1)
        {
            return children[0]->to_string() + "[]";
        }
        else
        {
            return children[0]->to_string() + "[" + children[1]->to_string() + "]";
        }
    }
    case Access:
    {
        return children[0]->to_string() + "." + token.value;
    }
    case Cast:
    {
        return "(" + children[0]->to_string() + ") as " + children[1]->to_string();
    }
    case Dict:
    {
        std::string result = "{ ";
        for (auto &child : children)
        {
            result += child->to_string();

            if (&child != &children.back())
                result += ", ";
        }
        result += " }";
        return result;
    }
    case DictPair:
    {
        return children[0]->to_string() + ": " + children[1]->to_string();
    }
    case Instantiation:
    {
        std::string result = "new " + children[0]->to_string() + " " + children[1]->to_string();
        return result;
    }
    case StructBraceInitializer:
    {
        std::string result = "{ ";

        for (auto &child : children)
        {
            result += child->to_string();

            if (&child != &children.back())
                result += ", ";
        }

        result += " }";
        return result;
    }
    case StructBraceInitializerPair:
    {
        return "." + token.value + " = " + children[0]->to_string();
    }
    case Optional:
    {
        return "(" + children[0]->to_string() + ")?";
    }
    case Generic:
    {
        std::string result = children[0]->to_string() + "<";
        for (auto &child : children[1]->children)
        {
            result += child->to_string();

            if (&child != &children[1]->children.back())
                result += ", ";
        }
        result += ">";
        return result;
    }
    case List:
    {
        return "List[" + children[0]->to_string() + "]";
    }
    case TypeList:
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
    default:
    {
        return "Unknown";
    }
    }
}