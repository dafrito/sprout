#ifndef SPROUT_LLVM_HEADER
#define SPROUT_LLVM_HEADER

#include <functional>

namespace sprout {

enum class ASTType {
    Unknown,
    NumberLiteral,
    Add,
    Multiply,
    Divide,
    Subtract,
    Exponent,
    Or,
    And,
    Function,
    Variable,
    ArgumentList,
    Call,
    If
};

int compare(const ASTType& left, const ASTType& right);

} // namespace sprout

namespace std {

template<>
struct hash<sprout::ASTType> {

    int operator()(const sprout::ASTType& type) const
    {
        return static_cast<int>(type);
    }
};

std::ostream& operator<<(std::ostream& stream, const sprout::ASTType& type);

} // namespace std

#endif // SPROUT_LLVM_HEADER
