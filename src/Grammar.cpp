#include "Grammar.hpp"

namespace std {

std::ostream& operator<<(std::ostream& stream, const sprout::TokenType& type)
{
    using namespace sprout;

    std::unordered_map<TokenType, const char*> names = {
        { TokenType::Unknown, "Unknown" },
        { TokenType::Opaque, "Opaque" },
        { TokenType::Name, "Name" },
        { TokenType::Literal, "Literal" },

        { TokenType::Rule, "Rule" },
        { TokenType::TokenRule, "TokenRule" },
        { TokenType::GroupRule, "GroupRule" },

        { TokenType::OneOrMore, "OneOrMore" },
        { TokenType::Discard, "Discard" },
        { TokenType::Optional, "Optional" },
        { TokenType::Alternative, "Alternative" },
        { TokenType::Sequence, "Sequence" },
        { TokenType::Recursive, "Recursive" },

        { TokenType::ZeroOrMore, "ZeroOrMore" },
        { TokenType::Join, "Join" },
    };

    stream << names.at(type);
    return stream;
}

} // namespace std

// vim: set ts=4 sw=4 :
