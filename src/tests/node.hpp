#ifndef SPROUT_TEST_NODE_HEADER
#define SPROUT_TEST_NODE_HEADER

#include <functional>

enum class TType {
    Unknown,
    Name,
    Access,
    Add,
    Subtract,
    Multiply,
    Divide,
    Exponent,
    Noop
};


int operatorPrecedence(const TType& type);

namespace std {

template<>
struct hash<TType> {
    int operator()(const TType& type) const
    {
        return static_cast<int>(type);
    }
};

std::ostream& operator<<(std::ostream& stream, const TType& type);

} // namespace std

#endif // SPROUT_TEST_NODE_HEADER
