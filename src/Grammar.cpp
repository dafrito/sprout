#include "Grammar.hpp"

namespace std {

std::ostream& operator<<(std::ostream& stream, const sprout::TokenType& type)
{
    using namespace sprout;

    std::unordered_map<TokenType, const char*> names = {
        { TokenType::Unknown, "Unknown" },
        { TokenType::Name, "Name" },
        { TokenType::Fundamental, "Fundamental" },
        { TokenType::StringLiteral, "StringLiteral" },
        { TokenType::TokenRule, "TokenRule" },
        { TokenType::Rule, "Rule" },
        { TokenType::GroupRule, "GroupRule" },
        { TokenType::ZeroOrMore, "ZeroOrMore" },
        { TokenType::OneOrMore, "OneOrMore" },
        { TokenType::Discard, "Discard" },
        { TokenType::Optional, "Optional" },
        { TokenType::Alternative, "Alternative" },
        { TokenType::Join, "Join" },
        { TokenType::Sequence, "Sequence" },
        { TokenType::Recursive, "Recursive" },
    };

    stream << names.at(type);
    return stream;
}

} // namespace std

// vim: set ts=4 sw=4 :
