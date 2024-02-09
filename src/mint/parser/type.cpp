
#include "mint/parser.hpp"

Unique<Ast> Parser::parse_type_list()
{
    return std::make_unique<Ast>(Ast::ExprList);
}