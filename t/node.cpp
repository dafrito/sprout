#include <grammar/Node.hpp>

#include "node.hpp"
#include "init.hpp"

using namespace sprout;
typedef grammar::Node<TType, QString> TNode;

BOOST_AUTO_TEST_CASE(testEquals)
{
    BOOST_CHECK_EQUAL(
        TNode(TType::Name, "Foo"),
        TNode(TType::Name, "Foo")
    );

    BOOST_CHECK(
        TNode(TType::Name, "Foo") != TNode(TType::Access, "Foo")
    );

    BOOST_CHECK(
        TNode(TType::Name, "Foo") != TNode(TType::Name, "Bar")
    );

    BOOST_CHECK_EQUAL(
        TNode(TType::Access, "Foo", {
            TNode(TType::Name, "Bar")
        }),
        TNode(TType::Access, "Foo", {
            TNode(TType::Name, "Bar")
        })
    );

    BOOST_CHECK(
        TNode(TType::Access, "Foo", {
            TNode(TType::Name, "Bar")
        }) !=
        TNode(TType::Access, "Foo", {
            TNode(TType::Name, "Zebra")
        })
    );
}

namespace std {

std::ostream& operator<<(std::ostream& stream, const TType& type)
{
    switch (type) {
        case TType::Unknown: return stream << "Unknown";
        case TType::Name: return stream << "Name";
        case TType::Access: return stream << "Access";
        default:
            std::stringstream str;
            str << "Unhandled type " << static_cast<int>(type) << std::endl;
            throw std::runtime_error(str.str());
    }
}

} // namespace std
