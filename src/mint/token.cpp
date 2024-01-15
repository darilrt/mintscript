#include "mint/token.hpp"

std::string Location::to_string() const
{
    return file + ":" + std::to_string(line) + ":" + std::to_string(column);
}

std::string Token::to_string() const
{
    std::string tokentType = "Unknown";

    switch (type)
    {
    case Type::Unknown:
    {
        tokentType = "Unknown";
        break;
    }
    case Type::Int:
    {
        tokentType = "Int";
        break;
    }
    case Type::Float:
    {
        tokentType = "Float";
        break;
    }
    case Type::String:
    {
        tokentType = "String";
        break;
    }
    case Type::Identifier:
    {
        tokentType = "Identifier";
        break;
    }
    case Type::Null:
    {
        tokentType = "Null";
        break;
    }
    case Type::True:
    {
        tokentType = "True";
        break;
    }
    case Type::False:
    {
        tokentType = "False";
        break;
    }
    case Type::If:
    {
        tokentType = "If";
        break;
    }
    case Type::Else:
    {
        tokentType = "Else";
        break;
    }
    case Type::Elif:
    {
        tokentType = "Elif";
        break;
    }
    case Type::While:
    {
        tokentType = "While";
        break;
    }
    case Type::For:
    {
        tokentType = "For";
        break;
    }
    case Type::In:
    {
        tokentType = "In";
        break;
    }
    case Type::Break:
    {
        tokentType = "Break";
        break;
    }
    case Type::Continue:
    {
        tokentType = "Continue";
        break;
    }
    case Type::Return:
    {
        tokentType = "Return";
        break;
    }
    case Type::Class:
    {
        tokentType = "Class";
        break;
    }
    case Type::Super:
    {
        tokentType = "Super";
        break;
    }
    case Type::As:
    {
        tokentType = "As";
        break;
    }
    case Type::Interface:
    {
        tokentType = "Interface";
        break;
    }
    case Type::Enum:
    {
        tokentType = "Enum";
        break;
    }
    case Type::Import:
    {
        tokentType = "Import";
        break;
    }
    case Type::Mut:
    {
        tokentType = "Mut";
        break;
    }
    case Type::Let:
    {
        tokentType = "Let";
        break;
    }
    case Type::Impl:
    {
        tokentType = "Impl";
        break;
    }
    case Type::Ref:
    {
        tokentType = "Ref";
        break;
    }
    case Type::Export:
    {
        tokentType = "Export";
        break;
    }
    case Type::Plus:
    {
        tokentType = "Plus";
        break;
    }
    case Type::Minus:
    {
        tokentType = "Minus";
        break;
    }
    case Type::Star:
    {
        tokentType = "Star";
        break;
    }
    case Type::Slash:
    {
        tokentType = "Slash";
        break;
    }
    case Type::Percent:
    {
        tokentType = "Percent";
        break;
    }
    case Type::Caret:
    {
        tokentType = "Caret";
        break;
    }
    case Type::Tilde:
    {
        tokentType = "Tilde";
        break;
    }
    case Type::Not:
    {
        tokentType = "Not";
        break;
    }
    case Type::Amp:
    {
        tokentType = "Amp";
        break;
    }
    case Type::Pipe:
    {
        tokentType = "Pipe";
        break;
    }
    case Type::Less:
    {
        tokentType = "Less";
        break;
    }
    case Type::Greater:
    {
        tokentType = "Greater";
        break;
    }
    case Type::Dot:
    {
        tokentType = "Dot";
        break;
    }
    case Type::Comma:
    {
        tokentType = "Comma";
        break;
    }
    case Type::Colon:
    {
        tokentType = "Colon";
        break;
    }
    case Type::Semicolon:
    {
        tokentType = "Semicolon";
        break;
    }
    case Type::Question:
    {
        tokentType = "Question";
        break;
    }
    case Type::At:
    {
        tokentType = "At";
        break;
    }
    case Type::Dollar:
    {
        tokentType = "Dollar";
        break;
    }
    case Type::Hash:
    {
        tokentType = "Hash";
        break;
    }
    case Type::LParen:
    {
        tokentType = "LParen";
        break;
    }
    case Type::RParen:
    {
        tokentType = "RParen";
        break;
    }
    case Type::LBracket:
    {
        tokentType = "LBracket";
        break;
    }
    case Type::RBracket:
    {
        tokentType = "RBracket";
        break;
    }
    case Type::LBrace:
    {
        tokentType = "LBrace";
        break;
    }
    case Type::RBrace:
    {
        tokentType = "RBrace";
        break;
    }
    case Type::Arrow:
    {
        tokentType = "Arrow";
        break;
    }
    case Type::Ellipsis:
    {
        tokentType = "Ellipsis";
        break;
    }
    case Type::Scope:
    {
        tokentType = "Scope";
        break;
    }
    case Type::Equal:
    {
        tokentType = "Equal";
        break;
    }
    case Type::PlusPlus:
    {
        tokentType = "PlusPlus";
        break;
    }
    case Type::MinusMinus:
    {
        tokentType = "MinusMinus";
        break;
    }
    case Type::AmpAmp:
    {
        tokentType = "AmpAmp";
        break;
    }
    case Type::PipePipe:
    {
        tokentType = "PipePipe";
        break;
    }
    case Type::EqualEqual:
    {
        tokentType = "EqualEqual";
        break;
    }
    case Type::LessLess:
    {
        tokentType = "LessLess";
        break;
    }
    case Type::GreaterGreater:
    {
        tokentType = "GreaterGreater";
        break;
    }
    case Type::PlusAssign:
    {
        tokentType = "PlusAssign";
        break;
    }
    case Type::MinusAssign:
    {
        tokentType = "MinusAssign";
        break;
    }
    case Type::StarAssign:
    {
        tokentType = "StarAssign";
        break;
    }
    case Type::SlashAssign:
    {
        tokentType = "SlashAssign";
        break;
    }
    case Type::ColonAssign:
    {
        tokentType = "ColonAssign";
        break;
    }
    case Type::PercentAssign:
    {
        tokentType = "PercentAssign";
        break;
    }
    case Type::CaretAssign:
    {
        tokentType = "CaretAssign";
        break;
    }
    case Type::AmpAssign:
    {
        tokentType = "AmpAssign";
        break;
    }
    case Type::PipeAssign:
    {
        tokentType = "PipeAssign";
        break;
    }
    case Type::LessLessAssign:
    {
        tokentType = "LessLessAssign";
        break;
    }
    case Type::GreaterGreaterAssign:
    {
        tokentType = "GreaterGreaterAssign";
        break;
    }
    case Type::EqualEqualEqual:
    {
        tokentType = "EqualEqualEqual";
        break;
    }
    case Type::NewLine:
    {
        tokentType = "NewLine";
        break;
    }
    case Type::EndFile:
    {
        tokentType = "EndFile";
        break;
    }
    case Type::NotEqual:
    {
        tokentType = "NotEqual";
        break;
    }
    case Type::SlashSlash:
    {
        tokentType = "SlashSlash";
        break;
    }
    }

    return tokentType + "(" + value + ")";
}

std::ostream &operator<<(std::ostream &os, const Token &token)
{
    os << token.to_string();
    return os;
}