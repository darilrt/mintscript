#include <memory>
#include <vector>
#include <string>

#include "mint/error.hpp"
#include "mint/parser.hpp"
#include "mint/token.hpp"
#include "mint/token_buffer.hpp"

Parser::Parser(const std::string &file_name)
{
    file.open(file_name);
    if (!file.is_open())
    {
        Error::error("Could not open file", Location{
                                                .line = 0,
                                                .column = 0,
                                                .file = file_name});
    }
    buffer = TokenBuffer(
        (std::streambuf *)file.rdbuf(),
        file_name);
}

std::unique_ptr<Ast> Parser::parse()
{
    return parse_program();
}