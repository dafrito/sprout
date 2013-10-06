#ifndef SPROUT_TEST_NODE_HEADER
#define SPROUT_TEST_NODE_HEADER

enum class TType {
    Unknown,
    Name,
    Access,
    Add,
    Subtract,
    Multiply,
    Divide,
    Exponent
};

int operatorPrecedence(const TType& type);

namespace std {

std::ostream& operator<<(std::ostream& stream, const TType& type);

} // namespace std

#endif // SPROUT_TEST_NODE_HEADER
